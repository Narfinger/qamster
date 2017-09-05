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
#[macro_use] extern crate error_chain;

mod errors {
    error_chain!{
    }
}
use errors::*;

use ansi_term::Colour::{Blue,Red,Purple,Green};
use chrono::TimeZone;
use clap::{App, Arg};
use futures::{Future};
use futures_cpupool::CpuPool;

//define typed header for reqwest
header! { (XPassword, "x-password") => [String] }

//these are temporary
static SITE: &'static str = include_str!("../../frontend_url.txt");
static PASSWORD: &'static str = include_str!("../../password.txt");

enum Endpoint {
    Current, // /current/
    List, // /list/
    Status, // /status/
    Total,  // /total/
    Start(String), // /start/
    Stop,   // /stop/
    Fuzzy(String), // /fuzzy/
}

fn time_to_local_format<'a>(s: chrono::NaiveDateTime) -> chrono::format::DelayedFormat<chrono::format::StrftimeItems<'a>> {
    chrono::Local.timestamp(s.timestamp(),0).format("%H:%M")
}

#[derive(Serialize, Deserialize, Debug)]
struct Task {
    id: i64,
    start: chrono::NaiveDateTime,
    end: chrono::NaiveDateTime,
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
               time_to_local_format(self.start),
               time_to_local_format(self.end),
               self.title,
               self.category,
               format_duration(&dur))
    }
}

#[derive(Debug,Serialize,Deserialize)]
struct RunningTask {
    id: i32,
    start: chrono::NaiveDateTime,
    title: String,
    category: String,
}

impl std::fmt::Display for RunningTask {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let secs = chrono::Utc::now().timestamp();
        let running_since = chrono::NaiveDateTime::from_timestamp(secs,0).signed_duration_since(self.start);
        write!(f, "{1} {0} {2} {0} {3: ^20} {0} Running since: {4}",
               Purple.paint("|"),
               time_to_local_format(self.start),
               self.title,
               self.category,
               format_duration(&running_since))
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

#[derive(Debug)]
enum QueryResult {
    List(Vec<Task>),
    Status(Vec<Status>),
    Current(Option<RunningTask>),
    None,
}

fn query_url(client: reqwest::Client, endpoint: &Endpoint) -> Result<QueryResult> { 
    let url = match *endpoint {
        Endpoint::Current => "/current/".to_owned(),
        Endpoint::List    => "/list/".to_owned(),
        Endpoint::Status  => "/status/".to_owned(),
        Endpoint::Total   => "/total/".to_owned(),
        Endpoint::Start(ref title) => format!("/start/?title={}", title.clone()),
        Endpoint::Stop    => "/stop/".to_owned(),
        Endpoint::Fuzzy(ref query) => format!("/fuzz/?q={}", query.clone()),
    };

    let res = client.get((SITE.to_owned() + url.as_str()).as_str())
        .header(XPassword(PASSWORD.to_owned()))
        .send();

    // let mut resstring = String::new();
    // res.unwrap().read_to_string(&mut resstring);
    // println!("{:?}", resstring);
    // Ok(QueryResult::None)
    //return the correct thing
    match *endpoint {
        Endpoint::Current    => res.and_then(|mut s| s.json()).map(QueryResult::Current),
        Endpoint::List       => res.and_then(|mut s| s.json()).map(QueryResult::List),
        Endpoint::Status     => res.and_then(|mut s| s.json()).map(QueryResult::Status),
        Endpoint::Total      => res.and_then(|mut s| s.json()).map(QueryResult::Status),
        Endpoint::Start(_)   => res                           .map(|_| QueryResult::None),
        Endpoint::Stop       => res                           .map(|_| QueryResult::None),
        Endpoint::Fuzzy(_)   => res.and_then(|mut s| s.json()).map(QueryResult::List),
    }.map_err(|e| Error::with_chain(e, "Server Connection Error"))
}

/// Starting a task
fn start_task(client: reqwest::Client, s: &str) {
    query_url(client, &Endpoint::Start(s.to_string())).expect("Error in starting task");
    //if let Some(s) = category {
    //    println!("Starting: {}@{}", Green.paint(task), Blue.paint(s));
    //} else {
    println!("Starting: {}", Green.paint(s));
    //}
}

/// Printing the list
fn print_list(client: reqwest::Client) {
    let pool = CpuPool::new_num_cpus();
    let c1 = client.clone();
    let c2 = client.clone();
    let c3 = client.clone();
    let c4 = client.clone();

    let current_future = pool.spawn_fn(|| {query_url(c1, &Endpoint::Current)});
    let list_future    = pool.spawn_fn(|| {query_url(c2, &Endpoint::List)});
    let status_future  = pool.spawn_fn(|| {query_url(c3, &Endpoint::Status)});
    let total_future   = pool.spawn_fn(|| {query_url(c4, &Endpoint::Total)});
       
    if let Ok(QueryResult::Current(t)) = current_future.wait() {
        if let Some(task) = t {
            println!("{0} {1}", Green.paint("Started"), task);
        } else {
            println!("{}", Purple.paint("No task running"));
        }
    }
    
    println!("{}", Green.paint("---------------------------------------------------------------------------"));
    if let Ok(QueryResult::List(s)) = list_future.wait() {
        for (i,item) in s.into_iter().enumerate() {
            println!{"{1} {0} {2}", Purple.paint("|"), i+1, item};
        }
        println!("{}", Green.paint("---------------------------------------------------------------------------"));
    }

    if let Ok(QueryResult::Status(mut total)) = total_future.wait() {
        if let Some(true_total) = total.pop() {
            if let Ok(QueryResult::Status(s)) = status_future.wait() {
                for item in s {
                    print!("{} ({:.2})", item, item.duration as f64/true_total.duration as f64);
                    print!(" | ");
                }
            }
            
            println!("{}", true_total);
        }
    }
    
    // working code
    // //println!("Starting to print list");
    // println!("{}", Green.paint("---------------------------------------------------------------------------"));
    // if let Ok(QueryResult::List(s)) = query_url(&Endpoint::List) {//list_future.wait() {
    //     for (i,item) in s.into_iter().enumerate() {
    //         println!{"{1} {0} {2}", Purple.paint("|"), i+1, item};
    //     }
    //     println!("{}", Green.paint("---------------------------------------------------------------------------"));
    // }
    // //println!("Printing status");
    // if let Ok(QueryResult::Status(s)) = query_url(&Endpoint::Status) {//status_future.wait() {
    //     for item in s {
    //         print!("{}", item);
    //         print!(" | ");
    //     }
    // }
    // if let Ok(QueryResult::Status(s)) = query_url(&Endpoint::Total) {//total_future.wait() {
    //     println!("{}", s[0]);
    // }
}

fn run(matches: clap::ArgMatches) -> Result<()> {    
    let client = reqwest::Client::new().expect("Could not create client");
    //checking online status
    {
        let res = client.get((SITE.to_owned()).as_str())
            .send();
        if let Err(e) = res {
            println!("{}", Red.paint("Cannot connect to host."));
            return Ok(())
        }
    }

    
    //continuing with the normal proram
    if matches.is_present("fuzzy") && matches.is_present("task") {
        let c1 = client.clone();
        if let Ok(QueryResult::List(l)) = query_url(c1, &Endpoint::Fuzzy(matches.value_of("task").unwrap().to_string())) {
            if l.len()==1 {
                let elem = &l[0];
                let taskstring = elem.title.clone() + "@" + elem.category.as_str();
                let c1 = client.clone();
                println!("{}: {}", Green.paint("Starting task"), taskstring);
                start_task(c1, taskstring.as_str());
            } else {
                println!("{}: {:?}", Red.paint("Not a unique match, found these matches"), l);
            }
        }

        println!("{}", Purple.paint("Not yet implemented (fuzzy task start)."));
    } else if matches.is_present("stop") {
        let c1 = client.clone();
        query_url(c1, &Endpoint::Stop).expect("Error in stop");
        println!("{}", Purple.paint("Stopping."));
        println!("{}", Green.paint("----------------------------------------------------------------------------"));
    } else if let Some(s) = matches.value_of("task") {
        let c1 = client.clone();
        start_task(c1, s);
        println!("{}", Green.paint("----------------------------------------------------------------------------"));
    } else if matches.is_present("fuzzy") {
        println!("{}", Red.paint("Need task to do a fuzzy add."));
        return Ok(())
    }

    {
        let c1 = client.clone();
        print_list(c1);
    }
    
    Ok(())
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

    if let Err(e) = run(matches) {
        println!("{}", e);
    }
}
