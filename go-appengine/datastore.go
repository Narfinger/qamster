package qamster

import (
	"net/http"
	"time"
	"appengine"
	"appengine/datastore"
	"strings"
	
	"strconv"
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
func ds_getStatusBar(r *http.Request) ([]Status) {
	//c := appengine.NewContext(r)
	var tasks []Task
	ds_getTasks(&tasks, r)
	var m = make(map[string](time.Duration))
	for i := 0; i< len(tasks); i++ {
		var t = tasks[i]
		var dur = t.End.Sub(t.Start)
		m[t.Category] = m[t.Category] + dur
	}

	var status []Status
	var totalsecs int64 = 0
	for k, dur := range m {
		var seconds = int64(dur.Seconds()) 
		var s = Status{ Title: k, Seconds: seconds, Percentage: 0 }
		status = append(status, s)
		totalsecs += seconds
	}

	//add total
	var totalstatus = Status{ Title: "total", Seconds: totalsecs, Percentage: 1}
	status = append(status, totalstatus)
	
	return status
}

func ds_queryTask(r *http.Request, query string) ([]string) {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("Tasks").Project("Title", "Category").Distinct()
	var t []Task
	var _, err = q.GetAll(c, &t)
	
	var res []string
	
	for i:=0; i< len(t); i++ {
		if strings.Contains(t[i].Title, query) {
			var s = t[i].Title + t[i].Category
			res = append(res, s)
		}
	}
	if err!=nil {
		res = append(res, err.Error())
	}
	res = append(res, strconv.Itoa((len(t))))
	return res
}
