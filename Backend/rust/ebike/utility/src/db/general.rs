use chrono::{Local, NaiveDateTime};
use deadpool_postgres::Pool;
use failure::Error;
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};

#[derive(PartialEq, Serialize, Deserialize)]
pub struct CarInfo {
    pub hash: String,
    pub code: String,
    pub status: Option<i64>,
    pub provide: Option<String>,
    pub speed: Option<f64>,
    pub gps: Option<Value>,
    pub gps_type: Option<i64>,
    pub time: Option<Value>,
    pub create_date: Option<NaiveDateTime>,
    pub update_date: Option<NaiveDateTime>,
    pub alert: Option<String>,
    pub remark: Option<String>,
}

#[derive(PartialEq, Serialize, Deserialize)]
pub struct StorageInfo {
    pub hash: String,
    pub code: String,
    pub provide: Option<String>,
    pub status: Option<i64>,
    pub sum: Option<i64>,
    pub cur: Option<i64>,
    pub gps: Option<Value>,
    pub gps_type: Option<i64>,
    pub create_date: Option<NaiveDateTime>,
    pub update_date: Option<NaiveDateTime>,
    pub alert: Option<String>,
    pub remark: Option<String>,
    pub points: Option<String>,
}

#[derive(PartialEq, Serialize, Deserialize)]
pub struct OptionsInfo {
    pub name: String,
    pub options: Value,
    pub level: i64,
}

pub async fn options(pool: &Pool) -> Result<Vec<OptionsInfo>, Error> {
    let client = pool.get().await?;

    let rows = client
        .query(
            "SELECT name, options, level FROM public.options WHERE delete = $1",
            &[&false],
        )
        .await
        .unwrap();

    if rows.len() <= 0 {
        Ok(vec![OptionsInfo {
            name: "".to_string(),
            options: json!({
                "system": 60,
                "alert": 60,
            }),
            level: 0,
        }])
    } else {
        let mut v: Vec<OptionsInfo> = vec![];
        for elem in rows {
            let option: Value = elem.get(1);
            let info = OptionsInfo {
                name: elem.get(0),
                options: option,
                level: elem.get(2),
            };
            v.push(info);
        }

        Ok(v)
    }
}

pub async fn car_add(info: &CarInfo, pool: &Pool) -> Result<bool, Error> {
    let client = pool.get().await?;

    let rows = client
        .query("SELECT code FROM public.car WHERE code = $1", &[&info.code])
        .await?;

    if rows.len() <= 0 {
        client
            .query(
                "INSERT INTO public.car VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12)",
                &[
                    &info.code,
                    &info.status,
                    &info.provide,
                    &info.speed,
                    &info.gps,
                    &info.time,
                    &Local::now().naive_local(),
                    &Local::now().naive_local(),
                    &false,
                    &info.alert,
                    &info.remark,
                    &info.gps_type,
                ],
            )
            .await?;

        Ok(true)
    } else {
        client
            .query(
                "UPDATE public.car SET status = $1, provide = $2, speed = $3, gps = $4, time = $5,
                update_date = $6, delete = $7, alert = $8, remark = $9, type = $10 WHERE code = $11",
                &[
                    &info.status,
                    &info.provide,
                    &info.speed,
                    &info.gps,
                    &info.time,
                    &Local::now().naive_local(),
                    &false,
                    &info.alert,
                    &info.remark,
                    &info.gps_type,
                    &info.code,
                ],
            )
            .await?;

        Ok(true)
    }
}

pub async fn car_query(
    code: &str,
    provide: &str,
    status: i64,
    time_start: &str,
    time_end: &str,
    pool: &Pool,
) -> Result<Vec<CarInfo>, Error> {
    let mut c = "".to_string();
    if code != "" {
        c = format!(" AND code = '{}'", code);
    }

    let mut p = "".to_string();
    if provide != "" {
        p = format!(" AND provide = '{}'", provide);
    }

    let mut s = "".to_string();
    if status != -1 {
        s = format!(" AND status = '{}'", status);
    }

    let mut pt = "".to_string();
    if time_start != "" && time_end != "" {
        pt = format!(
            " AND (time::json->'start')::TEXT LIKE '%{}%' AND (time::json->'end')::TEXT LIKE '%{}%'",
            time_start, time_end
        );
    } else if time_start != "" {
        pt = format!(" AND (time::json->'start')::TEXT LIKE '%{}%'", time_start);
    } else if time_end != "" {
        pt = format!(" AND (time::json->'end')::TEXT LIKE '%{}%'", time_end);
    }

    let client = pool.get().await?;

    let stat = client.prepare(&format!(
        "SELECT code, status, provide, speed, gps, type, time, alert, remark, create_date, update_date FROM public.car WHERE delete = false{}{}{}{}",
        c, p, s, pt
    )).await?;

    let rows = client.query(&stat, &[]).await?;

    if rows.len() <= 0 {
        Ok(vec![CarInfo {
            hash: "".to_string(),
            code: "".to_string(),
            status: Some(0),
            provide: Some("".to_string()),
            speed: Some(0_f64),
            gps: Some(json!({
                "lng": "".to_string(),
                "lat": "".to_string(),
            })),
            gps_type: Some(0),
            time: Some(json!({
                "start": "".to_string(),
                "end": "".to_string(),
            })),
            create_date: Some(Local::now().naive_local()),
            update_date: Some(Local::now().naive_local()),
            alert: Some("".to_string()),
            remark: Some("".to_string()),
        }])
    } else {
        let mut v: Vec<CarInfo> = vec![];
        for elem in rows {
            let gps: Value = elem.get(4);
            let time: Value = elem.get(6);
            let info = CarInfo {
                hash: "".to_string(),
                code: elem.get(0),
                status: elem.get(1),
                provide: elem.get(2),
                speed: elem.get(3),
                gps: Some(json!({
                    "lng": gps["lng"].as_str().unwrap_or(&"".to_string()),
                    "lat": gps["lat"].as_str().unwrap_or(&"".to_string()),
                })),
                gps_type: elem.get(5),
                time: Some(json!({
                    "start": time["start"].as_str().unwrap_or(&"".to_string()),
                    "end": time["end"].as_str().unwrap_or(&"".to_string()),
                })),
                create_date: elem.get(9),
                update_date: elem.get(10),
                alert: elem.get(7),
                remark: elem.get(8),
            };
            v.push(info);
        }

        Ok(v)
    }
}

pub async fn storage_query(
    code: &str,
    provide: &str,
    status: i64,
    pool: &Pool,
) -> Result<Vec<StorageInfo>, Error> {
    let mut c = "".to_string();
    if code != "" {
        c = format!(" AND code = '{}'", code);
    }

    let mut p = "".to_string();
    if provide != "" {
        p = format!(" AND provide = '{}'", provide);
    }

    let mut s = "".to_string();
    if status != -1 {
        s = format!(" AND status = '{}'", status);
    }

    let client = pool.get().await?;

    let stat = client.prepare(&format!(
        "SELECT code, status, provide, gps, type, sum, cur, alert, remark, points, create_date, update_date FROM public.storage WHERE delete = false{}{}{}",
        c, p, s
    )).await?;

    let rows = client.query(&stat, &[]).await?;

    if rows.len() <= 0 {
        Ok(vec![StorageInfo {
            hash: "".to_string(),
            code: "".to_string(),
            status: Some(0),
            provide: Some("".to_string()),
            gps: Some(json!({
                "lng": 0,
                "lat": 0,
            })),
            gps_type: Some(0),
            sum: Some(0),
            cur: Some(0),
            alert: Some("".to_string()),
            remark: Some("".to_string()),
            points: Some("".to_string()),
            create_date: Some(Local::now().naive_local()),
            update_date: Some(Local::now().naive_local()),
        }])
    } else {
        let mut v: Vec<StorageInfo> = vec![];
        for elem in rows {
            let gps: Value = elem.get(3);
            let info = StorageInfo {
                hash: "".to_string(),
                code: elem.get(0),
                status: elem.get(1),
                provide: elem.get(2),
                gps: Some(json!({
                    "lng": gps["lng"].as_str().unwrap_or(&"".to_string()),
                    "lat": gps["lat"].as_str().unwrap_or(&"".to_string()),
                })),
                gps_type: elem.get(4),
                sum: elem.get(5),
                cur: elem.get(6),
                alert: elem.get(7),
                remark: elem.get(8),
                points: elem.get(9),
                create_date: elem.get(10),
                update_date: elem.get(11),
            };
            v.push(info);
        }

        Ok(v)
    }
}
