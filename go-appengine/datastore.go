package qamster

import (
	"net/http"	
	"appengine"
	"appengine/datastore"
)

type isRunningStruct struct {
	isRunningField bool
	runningTask Task
}

func tasksKey(c appengine.Context) *datastore.Key {
        return datastore.NewKey(c, "Task", "default_task", 0, nil)
}

//func runningKey(c appengine.Context) *datastore.Key {
//	return datastore.NewKey(c, "Running", "default_running", 0, nil)
//}

//func runningTaskKey(c appengine.Context) *datastore.Key {
//	return datastore.NewKey(c, "RunningTask", "default_taskrunning", 0, nil)
//}

func ds_getRunning(w http.ResponseWriter, r *http.Request) bool {
	c := appengine.NewContext(r)
	var isRunning = isRunningStruct{}
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	if err := datastore.Get(c, key, &isRunning); err !=nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return false
	}
	return isRunning.isRunningField
}

func ds_setRunning(b bool, w http.ResponseWriter, r *http.Request) {
	c := appengine.NewContext(r)
	//get unique key
	var isRunning = isRunningStruct{isRunningField: b, runningTask: Task{}}
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	if _,err := datastore.Put(c, key, &isRunning); err !=nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
}

func ds_getRunningTask(r *http.Request) Task {
	c := appengine.NewContext(r)
	var runningTask = Task{}
	key := datastore.NewKey(c, "RunningTask", "runtask", 0, nil)
	datastore.Get(c, key, &runningTask)
	return runningTask
}

func ds_setRunningTask(t Task, r *http.Request) {
	c := appengine.NewContext(r)
	//get the unique key
	key := datastore.NewKey(c, "RunningTask", "runtask", 0, nil)
	datastore.Put(c, key, &t)
}

func ds_appendTask(t Task, r *http.Request) {
	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
	datastore.Put(c, key, &t)
}

// func ds_setTasks(t *Tasks, r *http.Request) {
// /*	c := appengine.NewContext(r)
// 	key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
// 	_, err := datastore.Put(c, key, t)
// 	if err != nil {
// 		http.Error(w, err.Error(), http.StatusInternalServerError)
// 		return
// 	}
// */
// }

func ds_getTasks(t *[]Task, r *http.Request) {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("Tasks").Order("-End")
	q.GetAll(c, t);
}
