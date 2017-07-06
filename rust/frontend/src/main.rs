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


use ansi_term::Colour::{Red,Purple,Green};
use chrono::DateTime;
use chrono::offset::{Local,Utc};
use clap::{App, Arg};

//these are temporary
static SITE: &'static str = "https://localhost:8000";
static PASSWORD: &'static str = "test";

enum Endpoint {
    List,
    Status,
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

fn format_duration(dur: &chrono::Duration) -> String {
    let hours = dur.num_hours();
    let min = dur.num_minutes() % 60;
    match hours {
        0 => format!("  {:02} min", min),
        s => format!("{}:{:02} min", s, min),
    }
}

impl std::fmt::Display for Task {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let dur = self.end.signed_duration_since(self.start);
        write!(f, "{1} {0} {2} {0} {3: ^20} {0} {4: ^20} {0} {5}",
               Purple.paint("|"),
               self.start.format("%H:%M"),
               self.end.format("%H:%M"),
               self.title,
               self.category,
               format_duration(&dur))
    }
}

#[derive(Serialize, Deserialize, Debug)]
struct Status {
    category: String,
    duration: i64,
}

impl std::fmt::Display for Status {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let dur = chrono::Duration::seconds(self.duration);
        write!(f, "{}:{}", self.category, format_duration(&dur))
    }
}

static MOCKLIST: &'static str = "
    [
        {\"start\": \"2017-07-06T10:55:38+00:00\", \"end\": \"2017-07-06T11:05:38+00:00\", \"title\": \"title1\", \"category\": \"category1\"},
        {\"start\": \"2017-07-06T11:05:38+00:00\", \"end\": \"2017-07-06T11:33:38+00:00\", \"title\": \"title2\", \"category\": \"category2\"},
        {\"start\": \"2017-07-06T13:55:38+00:00\", \"end\": \"2017-07-06T13:59:38+00:00\", \"title\": \"title1\", \"category\": \"category1\"},
        {\"start\": \"2017-07-06T14:00:38+00:00\", \"end\": \"2017-07-06T15:04:38+00:00\", \"title\": \"title1\", \"category\": \"category1\"}
    ]";

static MOCKSTATUS: &'static str = "[
        { \"category\": \"Category 1\", \"duration\": 120},
        { \"category\": \"Category 2\", \"duration\": 4},
        { \"category\": \"Category 3\", \"duration\": 34},
        { \"category\": \"Category 4\", \"duration\": 154}
]";

#[derive(Debug)]
enum QueryResult {
    List(Vec<Task>),
    Status(Vec<Status>),
    None,
}

fn mock_query_url(endpoint: Endpoint) -> Result<QueryResult, &'static str> {
    match endpoint {
        Endpoint::List   => Ok(QueryResult::List(serde_json::from_str(MOCKLIST).unwrap())),
        Endpoint::Status => Ok(QueryResult::Status(serde_json::from_str(MOCKSTATUS).unwrap())),
        Endpoint::Start(_) | Endpoint::Stop => Ok(QueryResult::None),
    }
}

fn query_url(endpoint: Endpoint) -> Result<QueryResult, &'static str> {
    // let client = reqwest::Client::new().expect("Could not create client");
    // let url = match endpoint {
    //     Endpoint::List  => "/timetable/?",
    //     Endpoint::Status => "/status/?",
    //     Endpoint::Start(ref task) => "/addTask/?",
    //     Endpoint::Stop  => "/stop/?"
    // }.to_owned() + "password=" + PASSWORD;

    // let res = client.get(url.as_str())
    //     .send();

    Ok(QueryResult::None)
}

fn print_list() {
    if let QueryResult::List(s) = mock_query_url(Endpoint::List).expect("No list returned") {

        for (i,item) in s.into_iter().enumerate() {
            println!{"{1} {0} {2}", Purple.paint("|"), i+1, item};
        }
        println!("{}", Green.paint("---------------------------------------------------------------------------"));
        if let QueryResult::Status(s) = mock_query_url(Endpoint::Status).expect("No status found") {
            for item in s {
                print!("{}", item);
                print!(" | ");
            }
        }
        
    } else {
        println!{"{}", Red.paint("Error in finding List")};
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
