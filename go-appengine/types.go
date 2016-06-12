package qamster

import (
	"time"
)

type IsRunningStruct struct {
	IsRunningField bool
	RunningTask    Task
}

type Task struct {
	Start    time.Time `json:"start"`
	End      time.Time `json:"end"`
	Title    string    `json:"title"`
	Category string    `json:"category"`
}

type Tasks []Task

type Status struct {
	Title      string  `json:"title"`
	Seconds    int64   `json:"seconds"`
	Percentage float64 `json:"percentage"`
}

type ChannelID struct {
	CId string `json:"cid"`
}
