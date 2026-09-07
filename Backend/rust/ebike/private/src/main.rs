use actix_cors::Cors;
use actix_identity::IdentityMiddleware;
use actix_session::{storage::CookieSessionStore, SessionMiddleware};
use actix_web::{cookie::Key, middleware, middleware::Logger, web::Data, App, HttpServer};
use anyhow::Error;
use chrono::Local;
use log::info;
use log4rs;
use serde::{Deserialize, Serialize};
use sqlx::{
    postgres::{PgConnectOptions, PgPoolOptions},
    ConnectOptions,
};
use std::{fs::File, io::Read, path::Path, str::FromStr, time::Duration};

mod db;
mod route;

#[derive(Serialize, Deserialize, Clone)]
pub struct Conf {
    pub url: String,
    pub pg_host: String,
    pub pg_name: String,
    pub pg_user: String,
    pub pg_pwd: String,
    pub mysql_host: String,
    pub mysql_name: String,
    pub mysql_user: String,
    pub mysql_pwd: String,
    pub refresh_rate: u64,
    pub register: bool,
    pub thread_num: usize,
}

// cargo install protobuf-codegen
// protoc --rust_out=. *.proto

#[actix_web::main]
async fn main() -> Result<(), Error> {
    log4rs::init_file("log4rs.yaml", Default::default()).unwrap();

    info!("========== 程序启动 ==========");

    let path = Path::new("config.json");
    let mut f = File::open(path)?;
    let mut s = String::new();
    f.read_to_string(&mut s)?;
    let config: Conf = serde_json::from_str(&s)?;

    let mut pg_options = PgConnectOptions::from_str(&format!(
        "postgres://{}:{}@{}/{}",
        config.pg_user, config.pg_pwd, config.pg_host, config.pg_name,
    ))?;
    pg_options
        .log_statements(log::LevelFilter::Debug)
        .log_slow_statements(log::LevelFilter::Debug, Duration::from_secs(1));
    let pg_pool = PgPoolOptions::new()
        .max_connections(100)
        .connect_with(pg_options)
        .await
        .unwrap();

    HttpServer::new(move || {
        App::new()
            .app_data(Data::new(pg_pool.clone()))
            .wrap(Logger::default())
            .wrap(Logger::new("%a %{User-Agent}i"))
            .wrap(
                Cors::default()
                    .allow_any_origin()
                    .allow_any_header()
                    .allow_any_method()
                    .supports_credentials()
                    .max_age(3600),
            )
            .wrap(
                middleware::DefaultHeaders::new()
                    .add(("Server", "CaNplay"))
                    .add(("date", Local::now().format("%Y-%m-%d %H:%M:%S").to_string())),
            )
            .wrap(middleware::Compress::default())
            .wrap(IdentityMiddleware::default())
            .wrap(
                SessionMiddleware::builder(CookieSessionStore::default(), Key::generate())
                    .cookie_name("ebkie".to_string())
                    .cookie_http_only(false)
                    .build(),
            )
            .service(route::general::login)
            .service(route::general::info)
            .service(route::general::loginout)
            .service(route::general::options)
            .service(route::general::car)
            .service(route::general::storage)
            .service(route::general::order)
    })
    .bind(&config.url)?
    .workers(config.thread_num)
    .run()
    .await?;

    info!("========== 程序退出 ==========");
    Ok(())
}
