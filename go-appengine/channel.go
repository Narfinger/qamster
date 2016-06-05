package qamster

import (
	"io"
	"net/http"
	"appengine"
	"appengine/channel"
)

const msgAddTask string = "addtask"
const msgStopTask string = "stoptask"

type ChannelMessage struct {
	Message string `json:"message"`
	Task    Task   `json:"task"`
}

func ch_createchannel(w http.ResponseWriter, r *http.Request) {
	c := appengine.NewContext(r)
	token, _ := channel.Create(c, "p")
	//ds_addChannelID(r, token)
	io.WriteString(w, token)
	//json.NewEncoder(w).Encode(token)	
}

func ch_clientConnected(w http.ResponseWriter, r *http.Request) {
	ds_addChannelID(r, r.FormValue("from"))
}

func ch_clientDisconnected(w http.ResponseWriter, r *http.Request) {
	ds_removeChannelID(r, r.FormValue("from"))
}
