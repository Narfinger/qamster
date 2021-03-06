package qamster

import (
	"io"
	"net/http"
	"math/rand"
	"strconv"
	"appengine"
	"appengine/datastore"
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
	var sid = strconv.Itoa(rand.Int())
	token, _ := channel.Create(c, sid)
	io.WriteString(w, token)

	
	//this is needed for some reason
	ds_addChannelID(r, sid)
	
}

func ch_clientConnected(w http.ResponseWriter, r *http.Request) {
//	ds_addChannelID(r, r.FormValue("from"))
}

func ch_clientDisconnected(w http.ResponseWriter, r *http.Request) {
	ds_removeChannelID(r, r.FormValue("from"))
}

func ch_sendToAll(msg interface{}, r *http.Request) {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("ChannelID").Distinct()
	var cids []ChannelID
	var _, _ = q.GetAll(c, &cids)
	for i:=0; i< len(cids); i++ {
		channel.SendJSON(c, cids[i].CId, msg)
	}
}

func ch_addTask(t Task, r *http.Request) {
	var m = ChannelMessage{Message: msgAddTask, Task: t}
	// str, _ := json.Marshal(m)
	ch_sendToAll(m, r)
}

func ch_stopTask(r *http.Request) {
	var m = ChannelMessage{Message: msgStopTask}
	ch_sendToAll(m,r)
}
