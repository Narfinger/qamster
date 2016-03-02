package qamster

import (
        "net/http"
//        "time"
	"html/template"
	"fmt"
        //"appengine"
        // "appengine/datastore"
        // "appengine/user"
)

const bodyTemplateString = `<html>
<head><title>TTTT</title></head>
<body>
this is from root</br>
{{template "body" .}}
</body>
</html>`

const rootTemplateString = `{{define "body"}}
<p><b>TTTTT</p></b>
{{end}}`



func init() {	
        http.HandleFunc("/", root)
}

// [START func_root]
func root(w http.ResponseWriter, r *http.Request) {
	
	bodyTemplate, err := template.New("body").Parse(bodyTemplateString)
	if err!= nil {
		http.Error(w, "first: " + err.Error(), http.StatusInternalServerError)
		fmt.Print("dat error")
		return
	}
	rootTemplate, err := template.Must(bodyTemplate.Clone()).Parse(rootTemplateString)
	if err!= nil {
		http.Error(w, "snd: " + err.Error(), http.StatusInternalServerError)
		fmt.Print("dat error2")
		return
	}

	// c := appengine.NewContext(r)
	if err := rootTemplate.Execute(w, ""); err != nil {
                http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
}
// [END func_root]
