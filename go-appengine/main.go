package qamster

import (
        "net/http"
//        "time"
        //"appengine"
	"html/template"
        // "appengine/datastore"
        // "appengine/user"
)

func init() {	
        //http.HandleFunc("/", root)
}

// [START func_root]
func root(w http.ResponseWriter, r *http.Request) {
	t, err := template.ParseFiles("templates/main.html")
	if err != nil {
		http.Error(w,err.Error(), http.StatusInternalServerError)
		return
	}
	// c := appengine.NewContext(r)
	if err := t.Execute(w, "root"); err != nil {
                http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}	
}
// [END func_root]
