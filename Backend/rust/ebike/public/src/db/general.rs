use chrono::{Datelike, Local, NaiveDateTime};
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use sqlx::{postgres::PgPool, Error};
use uuid::Uuid;

#[derive(PartialEq, Serialize, Deserialize, sqlx::FromRow)]
pub struct UserInfo {
    pub name: String,
    pub status: i64,
    pub level: i64,
}

#[derive(PartialEq, Serialize, Deserialize, sqlx::FromRow)]
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

#[derive(PartialEq, Serialize, Deserialize, sqlx::FromRow)]
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

#[derive(PartialEq, Serialize, Deserialize, sqlx::FromRow)]
pub struct OptionsInfo {
    pub name: String,
    pub options: Value,
    pub level: i64,
}

#[derive(PartialEq, Serialize, Deserialize, sqlx::FromRow)]
pub struct OrderInfo {
    pub hash: String,
    pub code: Option<String>,
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
    pub order: Option<String>,
    pub payable: Option<f64>,
    pub pay: Option<f64>,
    pub coupon: Option<f64>,
    pub refund: Option<f64>,
    pub pay_time: Option<NaiveDateTime>,
    pub pay_type: Option<i64>,
    pub pay_status: Option<i64>,
}

pub async fn options_query(pool: &PgPool) -> Result<Vec<OptionsInfo>, Error> {
    let rows = sqlx::query_as::<_, OptionsInfo>(r#"SELECT * FROM options"#)
        .fetch_all(pool)
        .await?;

    Ok(rows)
}

pub async fn car_add(info: &CarInfo, pool: &PgPool) -> Result<bool, Error> {
    // if !car_add_history(info, pool).await.unwrap() {
    //     return Ok(false);
    // }

    let rows = sqlx::query_as::<_, CarInfo>("SELECT code FROM public.car WHERE code = $1")
        .bind(&info.code)
        .fetch_all(pool)
        .await?;

    if rows.len() <= 0 {
        sqlx::query(
            "INSERT INTO public.car VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12)",
        )
        .bind(&info.code)
        .bind(info.status)
        .bind(&info.provide)
        .bind(info.speed)
        .bind(&info.gps)
        .bind(&info.time)
        .bind(Local::now().naive_local())
        .bind(Local::now().naive_local())
        .bind(false)
        .bind(&info.alert)
        .bind(&info.remark)
        .bind(info.gps_type)
        .execute(pool)
        .await?;

        Ok(true)
    } else {
        sqlx::query(
                "UPDATE public.car SET status = $1, provide = $2, speed = $3, gps = $4, time = $5,
                update_date = $6, delete = $7, alert = $8, remark = $9, type = $10 WHERE code = $11",
            )
            .bind(info.status)
            .bind(&info.provide)
            .bind(info.speed)
            .bind(&info.gps)
            .bind(&info.time)
            .bind(Local::now().naive_local())
            .bind(false)
            .bind(&info.alert)
            .bind(&info.remark)
            .bind(&info.gps_type)
            .bind(&info.code)
            .execute(pool)
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
    pool: &PgPool,
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

    let rows = sqlx::query_as::<_, CarInfo>(&format!(
        "SELECT code, status, provide, speed, gps, type, time, alert, remark, create_date, update_date FROM public.car WHERE delete = false{}{}{}{}",
        c, p, s, pt
    )).fetch_all(pool).await?;

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
        Ok(rows)
    }
}

async fn car_add_history(info: &CarInfo, pool: &PgPool) -> Result<bool, Error> {
    sqlx::query(
        &format!("INSERT INTO public.car_history_{}_{} VALUES ('{}', '{}', '{}', '{}', {}, '{}', '{}', '{}', '{}', {}, '{}', '{}', '{}')", 
        Local::now().year(),
        Local::now().month(),
        format!(
            "{}",
            Uuid::new_v4().simple().encode_lower(&mut Uuid::encode_buffer())
        ),
        info.code,
        info.status.unwrap_or(0_i64),
        info.provide.as_ref().unwrap_or(&("".to_string())),
        info.speed.unwrap_or(0_f64),
        info.gps.as_ref().unwrap_or(&(json!({"lng": "", "lat": ""}))),
        info.time.as_ref().unwrap_or(&(json!({"lng": "", "lat": ""}))),
        Local::now().format("%Y-%m-%d %H:%M:%S"),
        Local::now().format("%Y-%m-%d %H:%M:%S"),
        false,
        info.alert.as_ref().unwrap_or(&("".to_string())),
        info.remark.as_ref().unwrap_or(&("".to_string())),
        info.gps_type.unwrap_or(0_i64)
    )).execute(pool).await?;

    Ok(true)
}

pub async fn storage_query(
    code: &str,
    provide: &str,
    status: i64,
    pool: &PgPool,
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

    let rows = sqlx::query_as::<_, StorageInfo>(&format!(
        "SELECT code, status, provide, gps, type, sum, cur, alert, remark, points, create_date, update_date FROM public.storage WHERE delete = false{}{}{}",
        c, p, s
    )).fetch_all(pool).await?;

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
        Ok(rows)
    }
}

pub async fn order_add(info: &OrderInfo, pool: &PgPool) -> Result<String, Error> {
    if info.hash == "" {
        let hash = format!(
            "{}",
            Uuid::new_v4()
                .simple()
                .encode_lower(&mut Uuid::encode_buffer())
        );

        sqlx::query(
            &format!("INSERT INTO public.order_{}_{} VALUES ('{}', {}, '{}', {}, '{}', '{}', '{}', '{}', {}, '{}', 
            '{}', {}, '{}', {}, {}, {}, {}, '{}', {}, '{}', {})",
            Local::now().year(),
            Local::now().month(),
            info.code.as_ref().unwrap(),
            info.status.unwrap(),
            info.provide.as_ref().unwrap(),
            info.speed.unwrap(),
            info.gps.as_ref().unwrap(),
            info.time.as_ref().unwrap(),
            Local::now().naive_local(),
            Local::now().naive_local(),
            false,
            info.alert.as_ref().unwrap(),
            info.remark.as_ref().unwrap(),
            info.gps_type.unwrap(),
            hash,
            info.payable.unwrap(),
            info.pay.unwrap(),
            info.refund.unwrap(),
            info.coupon.unwrap(),
            info.order.as_ref().unwrap(),
            info.pay_type.unwrap(),
            info.pay_time.unwrap(),
            info.pay_status.unwrap(),
        )).execute(pool).await?;

        Ok(hash)
    } else {
        let rows = sqlx::query_as::<_, OrderInfo>(&format!(
            "SELECT hash FROM public.order_{}_{} WHERE hash = '{}' AND delete = false",
            Local::now().year(),
            Local::now().month(),
            info.hash,
        ))
        .fetch_all(pool)
        .await?;

        if rows.len() <= 0 {
            sqlx::query(
                &format!("UPDATE public.order_{}_{} SET code = '{}', status = {}, provide = {}, speed = {}, gps = '{}', 
                time = '{}', update_date = '{}', delete = {}, alert = '{}', remark = '{}', gps_type = {}, payable = {}, pay = {}, refund = {}, 
                coupon = {}, order = '{}', pay_type = {}, pay_time = '{}', pay_status = {} WHERE hash = '{}'",
                Local::now().year(),
                Local::now().month(),
                info.code.as_ref().unwrap(),
                info.status.unwrap(),
                info.provide.as_ref().unwrap(),
                info.speed.unwrap(),
                info.gps.as_ref().unwrap(),
                info.time.as_ref().unwrap(),
                Local::now().naive_local(),
                false,
                info.alert.as_ref().unwrap(),
                info.remark.as_ref().unwrap(),
                info.gps_type.unwrap(),
                info.payable.unwrap(),
                info.pay.unwrap(),
                info.refund.unwrap(),
                info.coupon.unwrap(),
                info.order.as_ref().unwrap(),
                info.pay_type.unwrap(),
                info.pay_time.unwrap(),
                info.pay_status.unwrap(),
                info.hash,
            )).execute(pool).await?;

            Ok(info.hash.clone())
        } else {
            Ok("error".to_string())
        }
    }
}
