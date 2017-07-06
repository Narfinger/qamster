extern crate ansi_term;
extern crate clap;
extern crate reqwest;

use ansi_term::Colour::Purple;
use clap::{App, Arg};

//these are temporary
static SITE: &'static str = "https://localhost:8000";
static PASSWORD: &'static str = "test";

enum Endpoint {
    List,
    Start,
    Stop,
}

fn query_url(endpoint: Endpoint, data: Option<String>) {
    let client = reqwest::Client::new().expect("Could not create client");
    let url = match endpoint {
        Endpoint::List  => "/timetable/",
        Endpoint::Start => "/addTask/",
        Endpoint::Stop  => "/stop/"
    };
    let complete_url = SITE.to_owned() + url;
    let res = client.post(complete_url.as_str())
        .body(PASSWORD)
        .send();
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
    
    if matches.is_present("fuzzy") & matches.is_present("task") {
        println!("{}", Purple.paint("Not yet implemented (fuzzy task start)."));
    } else if matches.is_present("stop") {
        println!("{}", Purple.paint("Not yet implemented (task stop)."));
    } else if matches.is_present("task") {
        println!("{}", Purple.paint("Not yet implemented (normal task start)."));
    } else {
        println!("{}", Purple.paint("Please select a command."));
    }

}
