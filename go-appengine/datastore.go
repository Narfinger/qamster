package qamster

import (
	"net/http"	
	"appengine"
	"appengine/datastore"
)

type IsRunningStruct struct {
	IsRunningField bool
	RunningTask Task
}

func tasksKey(c appengine.Context) *datastore.Key {
        return datastore.NewKey(c, "Task", "default_task", 0, nil)
}

func ds_getRunning(r *http.Request) (bool, Task) {
	c := appengine.NewContext(r)
	var isRunning = IsRunningStruct{} 
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	if err := datastore.Get(c, key, &isRunning); err != nil {
		return false, Task{}
	}
	return isRunning.IsRunningField, isRunning.RunningTask
}

func ds_setRunning(isRunning bool, rTask Task, r *http.Request) {
	c := appengine.NewContext(r)
	var rs = IsRunningStruct{IsRunningField: isRunning, RunningTask: rTask}
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	datastore.Put(c,key, &rs)
}

func ds_appendTask(t Task, r *http.Request) {
	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
	datastore.Put(c, key, &t)
}

func ds_getTasks(t *[]Task, r *http.Request) {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("Tasks").Order("End")
	q.GetAll(c, t);
}
