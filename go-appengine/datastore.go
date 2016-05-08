package qamster

import (
	"net/http"
	"time"
	"appengine"
	"appengine/datastore"
)

func tasksKey(c appengine.Context) *datastore.Key {
        return datastore.NewKey(c, "Task", "default_task", 0, nil)
}

//return running task
func ds_getRunning(r *http.Request) (bool, Task) {
	c := appengine.NewContext(r)
	var isRunning = IsRunningStruct{} 
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	if err := datastore.Get(c, key, &isRunning); err != nil {
		return false, Task{}
	}
	return isRunning.IsRunningField, isRunning.RunningTask
}

//set the running task
func ds_setRunning(isRunning bool, rTask Task, r *http.Request) {
	c := appengine.NewContext(r)
	var rs = IsRunningStruct{IsRunningField: isRunning, RunningTask: rTask}
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	datastore.Put(c,key, &rs)
}

//append a task
func ds_appendTask(t Task, r *http.Request) {
	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
	datastore.Put(c, key, &t)
}

// returns the current date with 0 hours, 0 minutes, 0 seconds and 0 milliseconds
func getCurrentDate() (time.Time) {
	var year,month,day = time.Now().Date()
	var t = time.Date(year, month, day, 0, 0, 0, 0, time.Local)
	return t
}

//get all current tasks and put them into t
func ds_getTasks(t *[]Task, r *http.Request) {
	c := appengine.NewContext(r)
	var today = getCurrentDate()
	q := datastore.NewQuery("Tasks").Order("End").Filter("End >=", today)
	q.GetAll(c, t);
}

//get the statusbar in a nice dictionary
func ds_getStatusBar(r *http.Request) (map[string](time.Duration)) {
	//c := appengine.NewContext(r)
	var tasks []Task
	ds_getTasks(&tasks, r)
	var m = make(map[string](time.Duration))
	for i := 0; i< len(tasks); i++ {
		var t = tasks[i]
		var dur = t.End.Sub(t.Start)
		m[t.Category] = m[t.Category] + dur
	}
	return m
}
