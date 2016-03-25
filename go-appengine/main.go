package qamster

import (
        "net/http"
	"encoding/json"
	"bytes"
	"time"
	
//	"appengine"
	// "appengine/log"
//	"appengine/datastore"
        // "appengine/user"
//	"strconv"
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

// func js_isRunning(w http.ResponseWriter, r *http.Request) {
// 	var isRunning, _ = ds_getRunning(r)
// 	json.NewEncoder(w).Encode(isRunning)
// }

//json function
func js_running(w http.ResponseWriter, r *http.Request) {
	var ir, rt = ds_getRunning(r)
	var s = IsRunningStruct{IsRunningField: ir, RunningTask: rt}
	json.NewEncoder(w).Encode(s)
}


func js_addTask(w http.ResponseWriter, r *http.Request) {
//	c := appengine.NewContext(r)
	var isRunning, runningTask = ds_getRunning(r)
	
	buf := new(bytes.Buffer)
	buf.ReadFrom(r.Body)
	s := buf.String()
	defer r.Body.Close()

	if isRunning {
		js_stop(w, r)
		// finishedTask := runningTask
		// finishedTask.End = time.Now();
		// key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
		// _, err := datastore.Put(c, key, &finishedTask)
		// if err != nil {
		// 	http.Error(w, err.Error(), http.StatusInternalServerError)
		// 	return
		// }	
	}
	runningTask = Task{Start: time.Now(), Title: s, Category: "test"}
	ds_setRunning(true, runningTask, r)
}

func js_stop(w http.ResponseWriter, r *http.Request) {
	var ir, finishedTask = ds_getRunning(r)
	if (ir) {
		finishedTask.End = time.Now()
		ds_appendTask(finishedTask, r)
		var runningTask = Task{}
		ds_setRunning(false,runningTask,r)
	}
}

func init() {
	http.HandleFunc("/go/running", js_running)
	http.HandleFunc("/go/timetable", js_timetable)
	http.HandleFunc("/go/addTask", js_addTask)
	http.HandleFunc("/go/stop", js_stop)


	//http.HandleFunc("/go/settrue", js_test_settrue)
	//http.HandleFunc("/", root)
}

