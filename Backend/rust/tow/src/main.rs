use actix_cors::Cors;
use actix_files::Files;
use actix_identity::IdentityMiddleware;
use actix_session::{SessionMiddleware, storage::CookieSessionStore};
use actix_web::{cookie::Key, middleware::{self, Logger}, web::{Data, JsonConfig}, App, HttpServer};
use chrono::Local;
use reqwest::Client;
use serde::Deserialize;
use sha2::{Sha512,Digest};
use std::{fs::File, io::Read, path::Path, str::FromStr, time::Duration};
use sqlx::{
    postgres::{PgConnectOptions, PgPoolOptions},
    ConnectOptions,
};
use anyhow::Error;
use log::info;

#[derive(Deserialize, Clone)]
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
    pub upload_path: String
}

mod db;
mod route;

// cargo install protobuf-codegen
// protoc --rust_out=. *.proto
// cargo build --release --target=x86_64-unknown-linux-musl

#[actix_web::main]
async fn main() -> Result<(), Error> {
    log4rs::init_file("log4rs.yaml", Default::default()).unwrap();

    info!("========== 程序启动 ==========");
    
    let path = Path::new("config.json");
    let mut f = File::open(path)?;
    let mut s = String::new();
    f.read_to_string(&mut s)?;
    let config: Conf = serde_json::from_str(&s)?;
    let config1: Conf = config.clone();

    let http_client = 
        Client::builder()
                    .user_agent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36 Edg/83.0.478.45")
                    .cookie_store(true)
                    .gzip(true)
                    .build()
                    .unwrap();

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
        
    let secret = "canplay";
    let mut hasher = Sha512::new();
    hasher.update(secret.as_bytes());
    let result = hasher.finalize();

    HttpServer::new(move || {
        App::new()
            .app_data(Data::new(http_client.clone()))
            .app_data(Data::new(config.clone()))
            .app_data(Data::new(pg_pool.clone()))
            .app_data(JsonConfig::default().limit(200000))
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
                    .add(("Date", Local::now().format("%Y-%m-%d %H:%M:%S").to_string()))
                    .add(("X-Frame-Options", "DENY"))
                    .add(("Referrer-Policy", "strict-origin-when-cross-origin"))
            )
            .wrap(middleware::Compress::default())
            .wrap(IdentityMiddleware::default())
            .wrap(
                SessionMiddleware::builder(CookieSessionStore::default(), Key::from(result.as_slice()))
                    .cookie_name("wstc".to_string())
                    .cookie_secure(false)
                    .build(),
            )
            .service(Files::new("/assets", "./web/assets")
                    .prefer_utf8(true)
                    .use_last_modified(true))
            .service(Files::new("/icons", "./web/icons")
                    .prefer_utf8(true)
                    .use_last_modified(true))
            .service(Files::new("/upload", "./web/upload")
                    .prefer_utf8(true)
                    .use_last_modified(true))
            .service(route::general::login)
            .service(route::general::loginout)
            .service(route::general::info)
            .service(route::general::user)
            .service(route::general::send)
            .service(route::general::car)
            .service(route::general::public)
            .service(route::general::proxy)
            .service(route::general::proxy_send)
            .service(route::options::options)
            .service(route::general::index)
    })
    .workers(config1.thread_num)
    .bind(&config1.url)?
    .run()
    .await?;

    log::info!("========== 程序退出 ==========");
    Ok(())
}
