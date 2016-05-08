package qamster

import (
	"time"
)


type IsRunningStruct struct {
	IsRunningField bool
	RunningTask Task
}

type Task struct {
	Start time.Time       `json:"start"`
	End time.Time         `json:"end"`
	Title string          `json:"title"`
	Category string       `json:"category"`
}
type Tasks []Task
