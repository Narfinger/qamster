#![cfg_attr(feature="clippy", feature(plugin))]
#![cfg_attr(feature="clippy", plugin(clippy))]
#![cfg_attr(feature="clippy", allow(needless_pass_by_value))] //rocket state uses this
extern crate chrono;
extern crate chrono_tz;
#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate serde_json;
extern crate rocket;
extern crate serde;

use diesel::sqlite::SqliteConnection;
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

#[get("/list/")]
fn list() -> JSON {
    
}

#[get("/status/")]
fn status() -> JSON {

}

#[get("/total/")]
fn total() -> JSON {

}

#[get("/start?<task>")]
fn start(task: Task) {
    
}

#[get("/stop/")]
fn stop() {
    
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
