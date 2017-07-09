#![cfg_attr(feature="clippy", feature(plugin))]
#![cfg_attr(feature="clippy", plugin(clippy))]
#![cfg_attr(feature="clippy", allow(needless_pass_by_value))] //rocket state uses this
#![feature(plugin,custom_derive)]
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
use std::ops::Deref;
use diesel::sqlite::SqliteConnection;
use diesel::{OrderDsl, LoadDsl};
use r2d2::Pool;
use r2d2_diesel::ConnectionManager;
use dotenv::dotenv;
use rocket::request::{State, Form, FromFormValue};
use rocket::response::{Redirect, NamedFile};
use rocket::response::content::Content;
use rocket::http::ContentType;
use rocket_contrib::JSON;

pub mod schema;

struct DB(Pool<ConnectionManager<SqliteConnection>>);

#[derive(Serialize,Deserialize,Queryable)]
struct Task {
    id: i32,
    start: chrono::NaiveDateTime,
    end: chrono::NaiveDateTime,
    title: String,
    category: String,
}

#[derive(FromForm)]
struct TaskForm {
    title: String,
    category: String,
}

#[derive(Serialize, Deserialize, Debug)]
struct Status {
    category: String,
    duration: i64,
}

#[get("/list")]
fn list(db: State<DB>) -> JSON<Vec<Task>> {
    use schema::task::dsl::*;
    let dbconn = db.0.get().expect("DB Pool problem");
    let tasks = task.order(start)
        .load(dbconn.deref())
        .unwrap();
    JSON(tasks)
}

#[get("/status")]
fn status(db: State<DB>) -> JSON<Vec<Status>> {
    JSON(vec![])
}

#[get("/total")]
fn total(db: State<DB>) -> JSON<Vec<Status>> {
    JSON(vec![])
}

#[get("/start?<task>")]
fn start(db: State<DB>, task: TaskForm) {
    
}

#[get("/stop")]
fn stop(db: State<DB>) {
    
}

fn main() {
    dotenv().ok();
    println!("Setting up database");
    let database_url = env::var("DATABASE_URL").expect("DATABASE_URL must be set");
    let config = r2d2::Config::default();
    let manager = ConnectionManager::<SqliteConnection>::new(database_url);
    let pool = r2d2::Pool::new(config, manager).expect("Failed to create pool.");

    rocket::ignite()
        .mount("/",
               routes![list, status, total, start, stop])
        .manage(DB(pool))
        .launch();
}
