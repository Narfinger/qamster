#![cfg_attr(feature="clippy", feature(plugin))]
#![cfg_attr(feature="clippy", plugin(clippy))]
extern crate ansi_term;
extern crate chrono;
extern crate clap;
extern crate reqwest;
extern crate serde;
#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate serde_json;


use ansi_term::Colour::Purple;
use chrono::DateTime;
use chrono::offset::{Local,Utc};
use clap::{App, Arg};

//these are temporary
static SITE: &'static str = "https://localhost:8000";
static PASSWORD: &'static str = "test";

enum Endpoint {
    List,
    Start(Task),
    Stop,
}

#[derive(Serialize, Deserialize, Debug)]
struct Task {
    start: DateTime<Utc>,
    end: DateTime<Utc>,
    title: String,
    category: String,
}

impl std::fmt::Display for Task {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let dur = self.end.signed_duration_since(self.start);
        let hours = dur.num_hours();
        let min = dur.num_minutes() % 60;
        let duration_string = match hours {
            s => format!("{}:{:02} min", s, min),
            0 => format!("{:02} min", min) 
        };
        
        write!(f, "{1} {0} {2} {0} {3: ^20} {0} {4: ^20} {0} {5}",
               Purple.paint("|"),
               self.start.format("%H:%M"),
               self.end.format("%H:%M"),
               self.title,
               self.category,
               duration_string)
    }
}

static MOCKLIST: &'static str = "
    [
        {\"start\": \"2017-07-06T10:55:38+00:00\", \"end\": \"2017-07-06T11:05:38+00:00\", \"title\": \"title1\", \"category\": \"category1\"},
        {\"start\": \"2017-07-06T11:05:38+00:00\", \"end\": \"2017-07-06T11:33:38+00:00\", \"title\": \"title2\", \"category\": \"category2\"},
        {\"start\": \"2017-07-06T13:55:38+00:00\", \"end\": \"2017-07-06T13:59:38+00:00\", \"title\": \"title1\", \"category\": \"category1\"},
        {\"start\": \"2017-07-06T14:00:38+00:00\", \"end\": \"2017-07-06T15:04:38+00:00\", \"title\": \"title1\", \"category\": \"category1\"}
    ]";

fn mock_query_url(endpoint: Endpoint) -> Option<Vec<Task>> {
    match endpoint {
        Endpoint::List => Some(serde_json::from_str(MOCKLIST).unwrap()),
        Endpoint::Start(_) | Endpoint::Stop => None,
    }
}

fn query_url(endpoint: Endpoint) -> Option<Vec<Task>> {
    let client = reqwest::Client::new().expect("Could not create client");
    let url = match endpoint {
        Endpoint::List  => "/timetable/?",
        Endpoint::Start(ref task) => "/addTask/?",
        Endpoint::Stop  => "/stop/?"
    }.to_owned() + "password=" + PASSWORD;

    let res = client.get(url.as_str())
        .send();

    None
}

fn print_list() {
    for (i,item) in mock_query_url(Endpoint::List).expect("No list returned").into_iter().enumerate() {
        println!{"{1} {0} {2}", Purple.paint("|"), i+1, item};
    }
}

fn main() {
    let matches = App::new("qamster")
        .about("Interfaces with qamster web service to control it")
        .arg(Arg::with_name("fuzzy")
             .long("fuzzy")
             .short("f")
             .help("First match is added as a task."))
        .arg(Arg::with_name("stop")
             .long("stop")
             .short("s")
             .help("stops the current task"))
        .arg(Arg::with_name("task")
             .help("Task to start (please use task@category)")
             .index(1))
        .get_matches();

    if matches.is_present("fuzzy") && matches.is_present("task") {
        println!("{}", Purple.paint("Not yet implemented (fuzzy task start)."));
    } else if matches.is_present("stop") {
        println!("{}", Purple.paint("Not yet implemented (task stop)."));
    } else if matches.is_present("task") {
        println!("{}", Purple.paint("Not yet implemented (normal task start)."));
    } else {
        print_list();
    }

}
