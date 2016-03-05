package qamster

import (
        "net/http"
	"encoding/json"
	"bytes"

	
	// "appengine"
	// "appengine/log"
        	// "appengine/datastore"
        // "appengine/user"
)

type Task struct {
	Start string    `json:"start"`
	End string      `json:"end"`
	Title string    `json:"title"`
	Category string `json:"category"`
	Runtime string  `json:"runtime"`
}
type Tasks []Task

var testdata = Tasks{
		Task{Start: "12:00", End: "12:15", Title: "test1", Category: "work",  Runtime: "15min"},
	 	Task{Start: "12:15", End: "12:30", Title: "test2", Category: "break", Runtime: "15min"},
	 	Task{Start: "12:30", End: "13:00", Title: "test3", Category: "work",  Runtime: "30min"},
		Task{Start: "13:00", End: "13:45", Title: "this is something", Category: "work", Runtime: "30min"},
	}

func timetable(w http.ResponseWriter, r *http.Request) {
	json.NewEncoder(w).Encode(testdata)
}

func addTask(w http.ResponseWriter, r *http.Request) {
	buf := new(bytes.Buffer)
	buf.ReadFrom(r.Body)
	s := buf.String()
	defer r.Body.Close()

	var t = Task{Start: "12:00", End: " 12:15", Title: s, Category: "work", Runtime: "15min"}
	testdata = append(testdata, t)
	//http.Response
}

func init() {
	http.HandleFunc("/go/timetable", timetable)
	http.HandleFunc("/go/addTask", addTask)
        //http.HandleFunc("/", root)
}

