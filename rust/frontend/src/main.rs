#![cfg_attr(feature="clippy", feature(plugin))]
#![cfg_attr(feature="clippy", plugin(clippy))]
extern crate ansi_term;
extern crate chrono;
extern crate clap;
extern crate futures;
extern crate futures_cpupool;
extern crate reqwest;
extern crate serde;
#[macro_use] extern crate serde_derive;
extern crate serde_json;
#[macro_use] extern crate hyper;


use ansi_term::Colour::{Blue,Red,Purple,Green};
use chrono::DateTime;
use chrono::offset::Utc;
use clap::{App, Arg};
use futures::Future;
use futures_cpupool::CpuPool;
use hyper::header::Headers;

//define typed header for reqwest
header! { (XPassword, "x-password") => [String] }

//these are temporary
static SITE: &'static str = include_str!("../../frontend_url.txt");
static PASSWORD: &'static str = include_str!("../../password.txt");
const DEBUG: bool = false;

enum Endpoint {
    List, // /list/
    Status, // /status/
    Total,  // /total/
    Start(String), // /start/
    Stop,   // /stop/
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
        0 => format!("   {:02} min", min),
        s => format!(" {}:{:02} min", s, min),
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

static MOCKTOTAL: &'static str = "[{\"category\": \"Total\", \"duration\": 102948}]";

#[derive(Debug)]
enum QueryResult {
    List(Vec<Task>),
    Status(Vec<Status>),
    None,
}

fn query_url(endpoint: &Endpoint) -> Result<QueryResult, reqwest::Error> {
    if DEBUG {
        match *endpoint {
            Endpoint::List   => Ok(QueryResult::List(serde_json::from_str(MOCKLIST).unwrap())),
            Endpoint::Status => Ok(QueryResult::Status(serde_json::from_str(MOCKSTATUS).unwrap())),
            Endpoint::Total  => Ok(QueryResult::Status(serde_json::from_str(MOCKTOTAL).unwrap())),
            Endpoint::Start(_) | Endpoint::Stop => Ok(QueryResult::None),
        }
    } else {
        
        let client = reqwest::Client::new().expect("Could not create client");
        let url = match *endpoint {
            Endpoint::List  => "/list/".to_owned(),
            Endpoint::Status => "/status/".to_owned(),
            Endpoint::Total => "/total/".to_owned(),
            Endpoint::Start(ref title) => format!("/start/?title={}&", title.clone()),
            Endpoint::Stop  => "/stop/".to_owned(),
        };
        
        let mut headers = Headers::new();

        headers.set(XPassword(PASSWORD.to_owned()));
        let res = client.get((SITE.to_owned() + url.as_str()).as_str())
            .header(headers)
            .send();
        
        //return the correct thing
        match *endpoint {
            Endpoint::List       => res.and_then(|mut s| s.json()).map(|s| QueryResult::List(s)),
            Endpoint::Status     => res.and_then(|mut s| s.json()).map(|s| QueryResult::Status(s)),
            Endpoint::Total      => res.and_then(|mut s| s.json()).map(|s| QueryResult::Status(vec!(s))),
            Endpoint::Start(_)   => res                           .map(|_| QueryResult::None),
            Endpoint::Stop       => res                           .map(|_| QueryResult::None),
        }
    }
}

/// Starting a task
fn start_task(s: &str) {
    query_url(&Endpoint::Start(s.to_string())).expect("Error in starting task");
    //if let Some(s) = category {
    //    println!("Starting: {}@{}", Green.paint(task), Blue.paint(s));
    //} else {
    println!("Starting: {}", Green.paint(s));
    //}
}

/// Printing the list
fn print_list() {
    let pool = CpuPool::new_num_cpus();
    // let list_future = pool.spawn_fn(  || {query_url(&Endpoint::List)})
    //     .map(|s| {
    //         match s {
    //             QueryResult::List(v) => Some(v),
    //             _ => None
    //         }});

    // let status_future = pool.spawn_fn( || {query_url(&Endpoint::Status)})
    //     .map(|s| {
    //         match s {
    //             QueryResult::Status(v) => Some(v),
    //             _ => None
    //         }});
    // let total_future  = pool.spawn_fn( || {query_url(&Endpoint::Total)})
    //     .map(|s| {
    //         match s {
    //             QueryResult::Status(v) => Some(v),
    //             _ => None
    //         }});

    println!("Starting to print list");
    if let Ok(QueryResult::List(s)) = query_url(&Endpoint::List) {//list_future.wait() {
        for (i,item) in s.into_iter().enumerate() {
            println!{"{1} {0} {2}", Purple.paint("|"), i+1, item};
        }
        println!("{}", Green.paint("---------------------------------------------------------------------------"));
    }
    println!("Printing status");
    if let Ok(QueryResult::Status(s)) = query_url(&Endpoint::Status) {//status_future.wait() {
        for item in s {
            print!("{}", item);
            print!(" | ");
        }
    }
    println!("Printint total");
    if let Ok(QueryResult::Status(s)) = query_url(&Endpoint::Total) {//total_future.wait() {
        println!("{}", s[0]);
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
        query_url(&Endpoint::Stop).expect("Error in stop");
        println!("{}", Purple.paint("Stopping"));
        println!("{}", Green.paint("----------------------------------------------------------------------------"));
    } else if let Some(s) = matches.value_of("task") {
        start_task(s);
        println!("{}", Green.paint("----------------------------------------------------------------------------"));
    } else if matches.is_present("fuzzy") {
        println!("{}", Red.paint("Need task to do a fuzzy add."));
        return
    }
    print_list();
}
