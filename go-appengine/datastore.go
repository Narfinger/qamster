package qamster

import (
	"fmt"
	"net/http"
	"strconv"
	"strings"
	"time"

	"golang.org/x/net/context"
	"google.golang.org/appengine"
	"google.golang.org/appengine/datastore"
	"google.golang.org/appengine/log"
)

func tasksKey(c context.Context) *datastore.Key {
	return datastore.NewKey(c, "Task", "default_task", 0, nil)
}

//return running task
func ds_getRunning(r *http.Request) (bool, Task) {
	c := appengine.NewContext(r)
	var isRunning = IsRunningStruct{}
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	if err := datastore.Get(c, key, &isRunning); err != nil {
		return false, Task{}
	}
	return isRunning.IsRunningField, isRunning.RunningTask
}

//set the running task
func ds_setRunning(isRunning bool, rTask Task, r *http.Request) {
	c := appengine.NewContext(r)
	var rs = IsRunningStruct{IsRunningField: isRunning, RunningTask: rTask}
	key := datastore.NewKey(c, "Running", "isrunning", 0, nil)
	datastore.Put(c, key, &rs)
}

//append a task
func ds_appendTask(t Task, r *http.Request) {
	c := appengine.NewContext(r)
	key := datastore.NewIncompleteKey(c, "Tasks", tasksKey(c))
	datastore.Put(c, key, &t)
}

// returns the current date with 0 hours, 0 minutes, 0 seconds and 0 milliseconds
func getCurrentDate() time.Time {
	var year, month, day = time.Now().Date()
	var t = time.Date(year, month, day, 0, 0, 0, 0, time.Local)
	return t
}

//get all current tasks and put them into t
func ds_getTasks(t *[]Task, r *http.Request) {
	c := appengine.NewContext(r)
	var today = getCurrentDate()
	q := datastore.NewQuery("Tasks").Order("End").Filter("End >=", today)
	q.GetAll(c, t)
}

//get the statusbar in a nice dictionary
func ds_getStatusBar(r *http.Request) []Status {
	//c := appengine.NewContext(r)
	var tasks []Task
	ds_getTasks(&tasks, r)
	var m = make(map[string](time.Duration))
	for i := 0; i < len(tasks); i++ {
		var t = tasks[i]
		var dur = t.End.Sub(t.Start)
		m[t.Category] = m[t.Category] + dur
	}

	var status []Status
	var totalsecs int64 = 0
	for k, dur := range m {
		var seconds = int64(dur.Seconds())
		var s = Status{Title: k, Seconds: seconds, Percentage: 0}
		status = append(status, s)
		totalsecs += seconds
	}

	for i, _ := range status {
		status[i].Percentage = float64(status[i].Seconds) / float64(totalsecs)
	}

	//add total
	var totalstatus = Status{Title: "total", Seconds: totalsecs, Percentage: 1}
	status = append(status, totalstatus)

	return status
}

func ds_queryTask(r *http.Request, query string) []string {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("Tasks").Project("Title", "Category").Distinct()
	var t []Task
	var _, err = q.GetAll(c, &t)

	var res []string

	for i := 0; i < len(t); i++ {
		if strings.Contains(t[i].Title, query) {
			var s = t[i].Title + "@" + t[i].Category
			res = append(res, s)
		}
	}
	if err != nil {
		res = append(res, err.Error())
	}
	return res
}

//channel methods
func ds_removeChannelID(r *http.Request, id string) {
	c := appengine.NewContext(r)
	//	c.Debugf("disconnected with: \"%s\"", id)

	q := datastore.NewQuery("ChannelID").Filter("CId =", id)
	var cids []ChannelID
	keys, _ := q.GetAll(c, &cids)
	//	c.Debugf("queries: (%s, %s)", len(cids), len(keys))
	//	c.Debugf("error: %s", err)

	for i := 0; i < len(keys); i++ {
		var key = keys[i]
		//c.Debugf("deleting with ds-id: %s", key)
		datastore.Delete(c, key)
	}
}

func ds_addChannelID(r *http.Request, id string) {
	c := appengine.NewContext(r)
	//        c.Debugf("new connection with: %s", id)
	var cid = ChannelID{CId: id}
	key := datastore.NewIncompleteKey(c, "ChannelID", nil)
	datastore.Put(c, key, &cid)
}

//returns true and the date if we found an entry and false otherwise
func ds_lastSummary(r *http.Request) (time.Time, bool) {
	c := appengine.NewContext(r)
	q := datastore.NewQuery("DailySummary").Project("Date").Order("Date")
	t := q.Run(c)
	var d time.Time
	_, err := t.Next(&d)
	if err == datastore.Done || err != nil {
		return d, false
	}
	return d, true
}

func summarizeDaysBetween(r *http.Request, start time.Time) {
	c := appengine.NewContext(r)
	var now = time.Now()

	var curYear, curMonth, curDay = start.Date()
	var m = make(map[string]map[string]time.Duration) //title, category, time

	for curYear <= now.Year() && curMonth <= now.Month() && curDay < now.Day() {
		var end = time.Date(curYear, curMonth, curDay, 0, 0, 0, 0, time.Local)
		var start = time.Date(curYear, curMonth, curDay, 23, 59, 59, 0, time.Local)
		var q = datastore.NewQuery("Tasks").Filter("End <=", end).Filter("Start <=", start)
		var t []Task
		var _, err = q.GetAll(c, &t)
		if err != nil {
			for i := 0; i < len(t); i++ {
				var cat = t[i].Category
				var title = t[i].Title
				var dur = t[i].End.Sub(t[i].Start)
				m[title][cat] = m[title][cat] + dur
			}
		}

		//add query to summary
		for k, v := range m {
			for k2, v2 := range v {
				var curDate = time.Date(curYear, curMonth, curDay, 0, 0, 0, 0, time.Local)
				var ds = DailySummary{
					Date:     curDate,
					Duration: v2,
					Title:    k,
					Category: k2}

				log.Infof(c, fmt.Sprintf("%v", ds))

				key := datastore.NewIncompleteKey(c, "DailySummary", nil)
				datastore.Put(c, key, &ds)
			}
		}

		//iterate onto next day
		var nextDate = time.Date(curYear, curMonth, curDay, 0, 0, 0, 0, time.Local).AddDate(0, 0, 1)
		curYear = nextDate.Year()
		curMonth = nextDate.Month()
		curDay = nextDate.Day()
	}

}

func ds_summarizeDaily(w http.ResponseWriter, r *http.Request) {
	c := appengine.NewContext(r)

	var d time.Time
	var err = false
	d, err = ds_lastSummary(r)
	if err == false {
		summarizeDaysBetween(r, d)
	} else {
		q := datastore.NewQuery("Task").Project("Start").Order("Start")
		t := q.Run(c)
		var first time.Time
		_, err := t.Next(&first)
		if err == datastore.Done || err != nil {
			log.Infof(c, "No Tasks found")
			return
		}
		summarizeDaysBetween(r, first)
	}
	log.Infof(c, strconv.FormatBool(err))
	log.Infof(c, d.Format(time.RFC1123))

	fmt.Fprintf(w, "OK")
}
