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

#[derive(Serializable,Queryable)]
struct Task {
    start: DateTime<Utc>,
    end: DateTime<Utc>,
    title: String,
    category: String,
}


#[get("/list/")]
fn list(db: State<DB>) -> JSON {
    use schema::task::dsl::*;
    let dbconn - db.get().expect("DB Pool problem");
    tasks.order(start)
        .load(dbconn.deref())
        .unwrap()
}

#[get("/status/")]
fn status(db: State<DB>) -> JSON {

}

#[get("/total/")]
fn total(db: State<DB>) -> JSON {

}

#[get("/start?<task>")]
fn start(db: State<DB>, task: Task) {
    
}

#[get("/stop/")]
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
