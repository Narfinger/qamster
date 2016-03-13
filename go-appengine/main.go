package qamster

import (
        "net/http"
	"encoding/json"
	"bytes"
	"time"
	
	"appengine"
	// "appengine/log"
	"appengine/datastore"
        // "appengine/user"
	"strconv"
)

type Task struct {
	Start time.Time       `json:"start"`
	End time.Time         `json:"end"`
	Title string          `json:"title"`
	Category string       `json:"category"`
	Runtime time.Duration `json:"runtime"`
}
type Tasks []Task

//json function
func js_timetable(w http.ResponseWriter, r *http.Request) {
        var tasks []Task

	//this is debug
	ds_getTasks(&tasks, r)

	json.NewEncoder(w).Encode(tasks)
}

func js_isRunning(w http.ResponseWriter, r *http.Request) {
	var isRunning = ds_getRunning(w,r)
	json.NewEncoder(w).Encode(isRunning)
}

//json function
func js_addTask(w http.ResponseWriter, r *http.Request) {
	c := appengine.NewContext(r)
	var isRunning = ds_getRunning(w,r)
	
	buf := new(bytes.Buffer)
	buf.ReadFrom(r.Body)
	s := buf.String()
	defer r.Body.Close()
	
	var runningTask = ds_getRunningTask(r)
	
	c.Infof(strconv.FormatBool(isRunning))
	if isRunning {
		finishedTask := runningTask
		finishedTask.End = time.Now();
		key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
		_, err := datastore.Put(c, key, &finishedTask)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}	
	} else {
		ds_setRunning(true,w,r)
	}
	runningTask = Task{Start: time.Now(), Title: s, Category: "test"}
	ds_setRunningTask(runningTask, r)
}

func js_stop(w http.ResponseWriter, r *http.Request) {
	var finishedTask = ds_getRunningTask(r)
	finishedTask.End = time.Now()
	ds_appendTask(finishedTask, r)
	
	
	ds_setRunning(false, w,r)
	var runningTask = Task{}
	ds_setRunningTask(runningTask, r)
}

func js_test_settrue(w http.ResponseWriter, r *http.Request) {
	ds_setRunning(true, w, r)
	return
}

func init() {
	http.HandleFunc("/go/isRunning", js_isRunning)
	http.HandleFunc("/go/timetable", js_timetable)
	http.HandleFunc("/go/addTask", js_addTask)
	http.HandleFunc("/go/stop", js_stop)


	http.HandleFunc("/go/settrue", js_test_settrue)
	//http.HandleFunc("/", root)
}

