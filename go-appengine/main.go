package qamster

import (
        "net/http"
//        "time"
        //"appengine"
	"encoding/json"
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

func timetable(w http.ResponseWriter, r *http.Request) {
	testdata := Tasks{
		Task{Start: "12:00", End: "12:15", Title: "test1", Category: "work",  Runtime: "15min"},
	 	Task{Start: "12:15", End: "12:30", Title: "test2", Category: "break", Runtime: "15min"},
	 	Task{Start: "12:30", End: "13:00", Title: "test3", Category: "work",  Runtime: "30min"},
		Task{Start: "13:00", End: "13:45", Title: "this is something", Category: "work", Runtime: "30min"},
	}
	json.NewEncoder(w).Encode(testdata)
}


func init() {
	http.HandleFunc("/go/timetable", timetable)
        //http.HandleFunc("/", root)
}

