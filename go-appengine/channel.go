package qamster

import (
	"net/http"
)

const msgAddTask string = "addtask"
const msgStopTask string = "stoptask"

type ChannelMessage struct {
	Message string `json:"message"`
	Task    Task   `json:"task"`
}

func ch_addTask(t Task, r *http.Request) {
	//var m = ChannelMessage{Message: msgAddTask, Task: t}
	// str, _ := json.Marshal(m)

}

func ch_stopTask(r *http.Request) {

}
