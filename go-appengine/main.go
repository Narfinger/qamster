package qamster

import (
        "net/http"
	"net/url"
	"encoding/json"
	"bytes"
	"time"
	"strings"
)

//json function
func js_timetable(w http.ResponseWriter, r *http.Request) {
        var tasks []Task

	//this is debug
	ds_getTasks(&tasks, r)

	json.NewEncoder(w).Encode(tasks)
}

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
 	}

	var splitstring = strings.Split(s, "@")
	if (len(splitstring) <= 1) {	
		runningTask = Task{Start: time.Now(), Title: splitstring[0], Category: ""}
	} else {
		runningTask = Task{Start: time.Now(), Title: splitstring[0], Category: splitstring[1]}
	}
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

func js_statusbar(w http.ResponseWriter, r *http.Request) {
	var status = ds_getStatusBar(r)
	json.NewEncoder(w).Encode(status)
}

func js_searchtask(w http.ResponseWriter, r *http.Request) {
	m, _ := url.ParseQuery(r.URL.RawQuery)

	var s []string
	if val, ok := m["q"]; ok== false {
		s = ds_queryTask(r, "")
	} else {
		s = ds_queryTask(r, val[0])
	}
	json.NewEncoder(w).Encode(s)
}
	
func init() {
	http.HandleFunc("/go/running", js_running)
	http.HandleFunc("/go/timetable", js_timetable)
	http.HandleFunc("/go/addTask", js_addTask)
	http.HandleFunc("/go/stop", js_stop)
	http.HandleFunc("/go/statusbar", js_statusbar)
	http.HandleFunc("/go/searchTask", js_searchtask)

	//http.HandleFunc("/go/settrue", js_test_settrue)
	//http.HandleFunc("/", root)
}

