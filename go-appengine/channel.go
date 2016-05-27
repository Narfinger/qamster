package qamster

import "net/http"

const msgAddTask string = "addtask"
const msgStopTask string = "stoptask"

type ChannelMessage struct {
	Message string `json:"message"`
	Task    Task   `json:"task"`
}

func ch_clientConnected(w http.ResponseWriter, r *http.Request) {
	ds_addChannelID(r, r.FormValue("from"))
	//	clientID := r.FormValue("from")
}

func ch_clientDisconnected(w http.ResponseWriter, r *http.Request) {
	ds_removeChannelID(r, r.FormValue("from"))
}
