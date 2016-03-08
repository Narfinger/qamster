package qamster

import (
	"net/http"	
	"appengine"
	"appengine/datastore"
)


func tasksKey(c appengine.Context) *datastore.Key {
        return datastore.NewKey(c, "Task", "default_task", 0, nil)
}

func runningKey(c appengine.Context) *datastore.Key {
	return datastore.NewKey(c, "Running", "default_running", 0, nil)
}

func runningTaskKey(c appengine.Context) *datastore.Key {
	return datastore.NewKey(c, "RunningTask", "default_taskrunning", 0, nil)
}

func ds_getRunning(r *http.Request) bool {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("Running").Limit(1)
	var running []bool
	if _, err := q.GetAll(c, &running); err != nil {
                //http.Error(w, err.Error(), http.StatusInternalServerError)
                return false
        }
	return running[0]
}

func ds_setRunning(b bool, r *http.Request) {
	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "Running", nil)
	_, err := datastore.Put(c, key, &b)
	if err != nil {
		//http.Error(w, err.Error(), http.StatusInternalServerError)
		return 
	}
}

func ds_getRunningTask(r *http.Request) Task {
/*	c := appengine.NewContext(r)
	q := datastore.NewQuery("RunningTask").Limit(1)
	var runningTask []Task
	if _, err := q.GetAll(c, &runningTask); err !=nil {
		//http.Error(w, err.Error(), http.StatusInternalServerError)
		return Task{}
	}
	return runningTask[0]
*/
	return Task{}
}

func ds_setRunningTask(t Task, r *http.Request) {
	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "RunningTask", runningTaskKey(c))
	datastore.Put(c, key, &t)
}

func ds_setTasks(t *Tasks, r *http.Request) {
/*	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
	_, err := datastore.Put(c, key, t)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
*/
}

func ds_getTasks(t *[]Task, r *http.Request) {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("Tasks")
	q.GetAll(c, t);
}
