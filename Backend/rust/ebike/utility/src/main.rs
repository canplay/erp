use chrono::Local;
use deadpool_postgres::{Config, ManagerConfig, RecyclingMethod, Runtime};
use failure::Error;
use geo::{algorithm::contains::Contains, point, Coordinate, LineString, MultiPolygon, Polygon};
use log::info;
use serde::Deserialize;
use std::{fs::File, io::Read, path::Path, process};
use tokio::time;
use tokio_postgres::NoTls;

mod db;

#[derive(Deserialize, Clone)]
pub struct Conf {
    pub url: String,
    pub pq_host: String,
    pub pq_name: String,
    pub pq_user: String,
    pub pq_pwd: String,
    pub mysql_host: String,
    pub mysql_name: String,
    pub mysql_user: String,
    pub mysql_pwd: String,
    pub refresh_rate: u64,
    pub register: bool,
    pub thread_num: usize,
}

#[tokio::main]
async fn main() -> Result<(), Error> {
    log4rs::init_file("log4rs.yaml", Default::default()).unwrap();

    ctrlc::set_handler(move || {
        info!("========== 程序退出 ==========");
        process::exit(0);
    })
    .expect("设置 Ctrl-C 失败");

    info!("========== 程序启动 ==========");

    let path = Path::new("config.json");
    let mut f = File::open(path)?;
    let mut s = String::new();
    f.read_to_string(&mut s)?;
    let config: Conf = serde_json::from_str(&s)?;

    let mut cfg = Config::new();
    cfg.host = Some(config.pq_host);
    cfg.dbname = Some(config.pq_name);
    cfg.user = Some(config.pq_user);
    cfg.password = Some(config.pq_pwd);
    cfg.manager = Some(ManagerConfig {
        recycling_method: RecyclingMethod::Fast,
    });
    let pool = cfg.create_pool(Some(Runtime::Tokio1), NoTls).unwrap();

    let options = db::general::options(&pool).await?;

    let mut system = time::interval(time::Duration::from_secs(
        options[0].options["system"].as_u64().unwrap(),
    ));

    loop {
        let mut num = 0;
        let alert = options[0].options["alert"].as_i64().unwrap();

        let cars = &db::general::car_query("", "", -1, "", "", &pool).await?;
        let storages = &db::general::storage_query("", "", -1, &pool).await?;

        let mut polygons: Vec<Polygon<f64>> = vec![];

        for storage in storages {
            let points = storage.points.as_ref().unwrap();
            let poly: Vec<&str> = points.split(';').collect();
            let mut pts: Vec<Coordinate<f64>> = vec![];

            for point in poly {
                let v: Vec<&str> = point.split(',').collect();
                pts.push(Coordinate {
                    x: v[0].parse::<f64>().unwrap(),
                    y: v[1].parse::<f64>().unwrap(),
                })
            }

            polygons.push(Polygon::new(LineString::from(pts.clone()), vec![]));
        }

        for car in cars {
            let gps = car.gps.clone().unwrap();
            let polygon = MultiPolygon::from(polygons.clone());
            let p = point!(x: gps["lng"].as_str().unwrap().parse::<f64>().unwrap(), y: gps["lat"].as_str().unwrap().parse::<f64>().unwrap());
            let r = polygon.contains(&p);

            let mut end = car.time.as_ref().unwrap()["end"].as_str().unwrap();
            if end == "" {
                end = "2000-01-01 00:00:00"
            }

            let time_end = chrono::NaiveDateTime::parse_from_str(end, "%Y-%m-%d %H:%M:%S").unwrap();
            let elapse = Local::now().naive_local() - time_end;

            if !r && elapse >= chrono::Duration::seconds(alert) && car.status == Some(1) {
                let _c = db::general::car_add(
                    &db::general::CarInfo {
                        hash: "".to_string(),
                        code: car.code.clone(),
                        status: Some(4),
                        provide: car.provide.clone(),
                        speed: car.speed,
                        gps: car.gps.clone(),
                        gps_type: car.gps_type,
                        time: car.time.clone(),
                        create_date: car.create_date,
                        update_date: car.update_date,
                        alert: Some("违停超时".to_string()),
                        remark: car.remark.clone(),
                    },
                    &pool,
                )
                .await?;
                num += 1;
            }
        }

        info!("警告：{} 违停超时", num);
        system.tick().await;
    }
}
