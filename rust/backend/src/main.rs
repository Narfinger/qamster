#![cfg_attr(feature="clippy", feature(plugin))]
#![cfg_attr(feature="clippy", plugin(clippy))]
#![cfg_attr(feature="clippy", allow(needless_pass_by_value))] //rocket state uses this
#![feature(plugin,custom_derive,custom_attribute)]
#![plugin(rocket_codegen)]
extern crate chrono;
#[macro_use] extern crate diesel;
#[macro_use] extern crate diesel_codegen;
extern crate dotenv;
extern crate rocket;
extern crate rocket_contrib;
extern crate r2d2;
extern crate r2d2_diesel;
extern crate serde;
#[macro_use]
extern crate serde_derive;
extern crate serde_json;


use std::env;
use std::collections::HashMap;
use std::ops::Deref;
use chrono::Datelike;
use diesel::sqlite::SqliteConnection;
use diesel::{GroupByDsl, ExecuteDsl, FilterDsl, ExpressionMethods,OrderDsl, LoadDsl, insert, delete, select};
use diesel::expression::exists;
use r2d2::Pool;
use r2d2_diesel::ConnectionManager;
use dotenv::dotenv;
use rocket::request::{State};
use rocket_contrib::Json;
pub mod schema;
use schema::{task,running_task};

struct DB(Pool<ConnectionManager<SqliteConnection>>);

#[derive(Queryable,Serialize,Deserialize,Debug)]
struct Task {
    id: i32,
    start: chrono::NaiveDateTime,
    end: chrono::NaiveDateTime,
    title: String,
    category: String,
}

#[derive(Debug,Insertable)]
#[table_name="task"]
struct NewTask {
    start: chrono::NaiveDateTime,
    end: chrono::NaiveDateTime,
    title: String,
    category: String,
}

#[derive(FromForm)]
struct TaskForm {
    title: String,
    password: String,
}

#[derive(FromForm)]
struct PasswordForm {
    password: String,
}

#[derive(Debug,Serialize,Queryable)]
struct RunningTask {
    id: i32,
    start: chrono::NaiveDateTime,
    title: String,
    category: String,
}

#[derive(Debug, Insertable)]
#[table_name="running_task"]
struct NewRunningTask {
    start: chrono::NaiveDateTime,
    title: String,
    category: String,
}

#[derive(Serialize,Deserialize,Debug)]
struct Status {
    category: String,
    duration: i64,
}

#[get("/list")]
fn list(db: State<DB>) -> Json<Vec<Task>> {
    use schema::task::dsl::*;
    let dbconn = db.0.get().expect("DB Pool problem");
    let now = chrono::offset::Utc::now().naive_utc();
    let today = chrono::NaiveDate::from_ymd(now.year(),now.month(),now.day()).and_hms(0,0,0);
    let tasks = task.order(start)
        .filter(start.ge(today))
        .load(dbconn.deref())
        .unwrap();

    Json(tasks)
}

fn get_status(db: State<DB>) -> Vec<Status> {
    use schema::task::dsl::*;
    let dbconn = db.0.get().expect("DB Pool problem");
    let now = chrono::offset::Utc::now().naive_utc();
    let today = chrono::NaiveDate::from_ymd(now.year(),now.month(),now.day()).and_hms(0,0,0);
    let tasks:Vec<Task> = task.filter(start.ge(today)).load(dbconn.deref()).expect("Error in finding tasks");//.group_by(category);
    println!("{:?}", tasks);

    let mut map:HashMap<String,i64> = HashMap::new();
    
    for t in tasks {
        let time_diff = t.end.timestamp() - t.start.timestamp();
        let time_add = time_diff + map.get(t.category.as_str()).unwrap_or(&0);
        println!("{:?}", t.category);
        map.insert(t.category.clone(), time_add);
    }
    println!("{:?}", map);

    let mut res = Vec::new();
    for (k,i) in map {
        let s = Status{category: k, duration: i};
        res.push(s);
    }

    res
}

#[get("/status")]
fn status(db: State<DB>) -> Json<Vec<Status>> {
    let stat = get_status(db);
    println!("{:?}", stat);

    Json(stat)
}

#[get("/total")]
fn total(db: State<DB>) -> Json<Vec<Status>> {
    let mut s = Status{category: "Total".to_owned(), duration: 0};
    let ss = get_status(db);
    for i in ss {
        s.duration += i.duration;
    }
    
    Json(vec![s])
}
/// Helper function for stopping a task
fn stop_task(db: State<DB>) {
    let dbconn = db.0.get().expect("DB Pool Problem");
    let rtask:&RunningTask = &running_task::table.load(dbconn.deref()).unwrap()[0];
    let completed_task = NewTask{ start: rtask.start,
                                  end: chrono::offset::Utc::now().naive_utc(),
                                  title: rtask.title.to_owned(),
                                  category: rtask.category.to_owned(),
    };
    //delete the task (whole table actually)
    delete(running_task::table).execute(dbconn.deref()).expect("Delete of running failed");
    
    //insert this
    insert(&completed_task).into(task::table).execute(dbconn.deref()).expect("Insert of completed failed");
}

#[get("/start?<taskform>")]
fn start(db: State<DB>, taskform: TaskForm) {
    let dbconn = db.0.get().expect("DB Pool Problem");
    
    let mut iterator = taskform.title.splitn(2,'@');
    let task = String::from(iterator.next().expect("No Task specified, empty string?"));
    let category = String::from(iterator.next().unwrap_or(""));

    let task_is_running = select(exists(running_task::table)).get_result(dbconn.deref());
    if Ok(true) == task_is_running {
        stop_task(db);
    }
    //add new running task
    let nrtask = NewRunningTask { start: chrono::offset::Utc::now().naive_utc(),
                                  title: task,
                                  category: category,
    };
    insert(&nrtask).into(running_task::table).execute(dbconn.deref()).expect("Insert of running failed");
    println!("starting something with: {:?}", nrtask);
}

#[get("/stop")]
fn stop(db: State<DB>) {
    stop_task(db);
}

fn main() {
    dotenv().ok();
    println!("Setting up database");
    let database_url = env::var("DATABASE_URL").expect("DATABASE_URL must be set");
    let config = r2d2::Config::default();
    let manager = ConnectionManager::<SqliteConnection>::new(database_url);
    let pool = r2d2::Pool::new(config, manager).expect("Failed to create pool.");

    //needs ssl
    rocket::ignite()
        .mount("/",
               routes![list, status, total, start, stop])
        .manage(DB(pool))
        .launch();
}
