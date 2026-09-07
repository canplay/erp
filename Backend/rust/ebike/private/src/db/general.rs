use chrono::{Datelike, Local, NaiveDateTime};
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use sqlx::{postgres::PgPool, Error};
use uuid::Uuid;

#[derive(PartialEq, Serialize, Deserialize, Clone, Debug, sqlx::FromRow)]
pub struct UserInfo {
    pub id: String,
    pub username: String,
    pub password: String,
    pub level: i64,
    pub name: Option<String>,
    pub create_user: Option<String>,
    pub create_date: Option<NaiveDateTime>,
    pub update_user: Option<String>,
    pub update_date: Option<NaiveDateTime>,
}

#[derive(PartialEq, Serialize, Deserialize, Clone, Debug, sqlx::FromRow)]
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

#[derive(PartialEq, Serialize, Deserialize, Clone, Debug, sqlx::FromRow)]
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

#[derive(PartialEq, Serialize, Deserialize, Clone, Debug, sqlx::FromRow)]
pub struct OptionsInfo {
    pub name: String,
    pub options: Value,
    pub level: i64,
    pub create_date: Option<NaiveDateTime>,
    pub update_date: Option<NaiveDateTime>,
}

#[derive(PartialEq, Serialize, Deserialize, Clone, Debug, sqlx::FromRow)]
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

pub async fn login(username: &str, password: &str, pool: &PgPool) -> Result<UserInfo, Error> {
    let rows = sqlx::query_as::<_, UserInfo>(
        "SELECT * FROM public.ebike_user WHERE username = $1 AND password = $2",
    )
    .bind(&username)
    .bind(&password)
    .fetch_one(pool)
    .await?;

    Ok(rows)
}

pub async fn user_info(id: String, pool: &PgPool) -> Result<UserInfo, Error> {
    let rows = sqlx::query_as::<_, UserInfo>("SELECT * FROM public.ebike_user WHERE id = $1")
        .bind(id)
        .fetch_one(pool)
        .await?;

    Ok(rows)
}

pub async fn options_query(pool: &PgPool) -> Result<Vec<OptionsInfo>, Error> {
    let rows =
        sqlx::query_as::<_, OptionsInfo>("SELECT * FROM public.ebike_options WHERE delete = false")
            .fetch_all(pool)
            .await?;

    if rows.len() <= 0 {
        Ok(vec![OptionsInfo {
            name: "".to_string(),
            options: json!({
                "system": 60,
                "alert": 30,
            }),
            level: 0,
            create_date: Some(Local::now().naive_local()),
            update_date: Some(Local::now().naive_local()),
        }])
    } else {
        Ok(rows)
    }
}

pub async fn options_update(info: &OptionsInfo, pool: &PgPool) -> Result<bool, Error> {
    let rows =
        sqlx::query_as::<_, OptionsInfo>("SELECT name FROM public.ebike_options WHERE name = $1")
            .bind(&info.name)
            .fetch_all(pool)
            .await?;

    if rows.len() <= 0 {
        sqlx::query("INSERT INTO public.ebike_options VALUES ($1, $2, $3, $4, $5, $6)")
            .bind(&info.name)
            .bind(&info.options)
            .bind(Local::now().naive_local())
            .bind(Local::now().naive_local())
            .bind(false)
            .bind(info.level)
            .execute(pool)
            .await?;

        Ok(true)
    } else {
        sqlx::query(
                "UPDATE public.ebike_options SET name = $1, options = $2, update_date = $3, delete = $4, level = $5 WHERE name = $6",
            )
            .bind(&info.name)
            .bind(&info.options)
            .bind(Local::now().naive_local())
            .bind(false)
            .bind(info.level)
            .bind(&info.name)
            .execute(pool)
            .await?;

        Ok(true)
    }
}

pub async fn car_add(info: &CarInfo, pool: &PgPool) -> Result<bool, Error> {
    if !car_add_history(info, pool).await.unwrap() {
        return Ok(false);
    }

    let rows = sqlx::query_as::<_, CarInfo>("SELECT code FROM public.ebike_car WHERE code = $1")
        .bind(&info.code)
        .fetch_all(pool)
        .await?;

    if rows.len() <= 0 {
        sqlx::query(
            "INSERT INTO public.ebike_car VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13)",
        )
        .bind(&info.code)
        .bind(&info.status)
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
        .bind(Uuid::new_v4().simple().encode_lower(&mut Uuid::encode_buffer()).to_string())
        .execute(pool)
        .await?;

        Ok(true)
    } else {
        sqlx::query(
                "UPDATE public.ebike_car SET status = $1, provide = $2, speed = $3, gps = $4, time = $5,
                update_date = $6, delete = $7, alert = $8, remark = $9, gps_type = $10 WHERE code = $11",
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
        .bind(info.gps_type)
        .bind(&info.code)
        .execute(pool)
        .await?;

        Ok(true)
    }
}

pub async fn car_del(code: &str, pool: &PgPool) -> Result<bool, Error> {
    sqlx::query("UPDATE public.ebike_car SET update_date = $1, delete = $2 WHERE code = $3")
        .bind(Local::now().naive_local())
        .bind(true)
        .bind(code)
        .execute(pool)
        .await?;

    Ok(true)
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
        "SELECT * FROM public.ebike_car WHERE delete = false{}{}{}{}",
        c, p, s, pt
    ))
    .fetch_all(pool)
    .await?;

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
        &format!("INSERT INTO public.ebike_car_history_{}_{} VALUES ('{}', '{}', '{}', '{}', {}, '{}', '{}', '{}', '{}', {}, '{}', '{}', '{}')", 
        Local::now().year(),
        Local::now().month(),
        Uuid::new_v4().simple().encode_lower(&mut Uuid::encode_buffer()),
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
        info.gps_type.unwrap_or(0_i64),
    )).execute(pool).await?;

    Ok(true)
}

pub async fn car_history(code: &str, pool: &PgPool) -> Result<Vec<CarInfo>, Error> {
    let rows = sqlx::query_as::<_, CarInfo>(&format!(
        "SELECT * FROM public.ebike_car_history_{}_{} WHERE code = '{}' AND delete = false",
        Local::now().year(),
        Local::now().month(),
        code,
    ))
    .fetch_all(pool)
    .await?;

    if rows.len() <= 0 {
        Ok(vec![CarInfo {
            hash: "".to_string(),
            code: "".to_string(),
            status: Some(0),
            provide: Some("".to_string()),
            speed: Some(0_f64),
            gps: Some(json!({
                "lng": 0,
                "lat": 0,
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

pub async fn car_alert(
    code: &str,
    provide: &str,
    status: i64,
    time: &str,
    alert: &str,
    remark: &str,
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

    let mut t = "".to_string();
    if time != "" {
        t = format!(" AND create_date LIKE '%{}%'", time);
    }

    let mut a = "".to_string();
    if alert == "" {
        a = " AND alert != ''".to_string();
    } else if alert != "" {
        a = format!(" AND alert LIKE '%{}%'", alert);
    }

    let mut r = "".to_string();
    if remark != "" {
        r = format!(" AND remark LIKE '%{}%'", remark);
    }

    let rows = sqlx::query_as::<_, CarInfo>(&format!(
        "SELECT * FROM public.ebike_car_history_{}_{} WHERE delete = false{}{}{}{}{}{}",
        Local::now().year(),
        Local::now().month(),
        c,
        p,
        s,
        t,
        a,
        r
    ))
    .fetch_all(pool)
    .await?;

    if rows.len() <= 0 {
        Ok(vec![CarInfo {
            hash: "".to_string(),
            code: "".to_string(),
            status: Some(0),
            provide: Some("".to_string()),
            speed: Some(0_f64),
            gps: Some(json!({
                "lng": 0,
                "lat": 0,
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

pub async fn storage_add(info: &StorageInfo, pool: &PgPool) -> Result<bool, Error> {
    if !storage_add_history(info, pool).await.unwrap() {
        return Ok(false);
    }

    let rows =
        sqlx::query_as::<_, StorageInfo>("SELECT code FROM public.ebike_storage WHERE code = $1")
            .bind(&info.code)
            .fetch_all(pool)
            .await?;

    if rows.len() <= 0 {
        sqlx::query(
                "INSERT INTO public.ebike_storage VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14)",
            )
            .bind(Uuid::new_v4().simple().encode_lower(&mut Uuid::encode_buffer()).to_string())
            .bind(&info.code)
            .bind(info.status)
            .bind(&info.provide)
            .bind(&info.gps)
            .bind(Local::now().naive_local())
            .bind(Local::now().naive_local())
            .bind(false)
            .bind(&info.alert)
            .bind(&info.remark)
            .bind(info.sum)
            .bind(info.cur)
            .bind(&info.points)
            .bind(info.gps_type)
            .execute(pool)
            .await?;

        Ok(true)
    } else {
        sqlx::query(
                "UPDATE public.ebike_storage SET provide = $1, gps = $2, update_date = $3, delete = $4, 
                alert = $5, remark = $6, sum = $7, cur = $8, gps_type = $9, status = $10 WHERE code = $11",
            )
            .bind(&info.provide)
            .bind(&info.gps)
            .bind(Local::now().naive_local())
            .bind(false)
            .bind(&info.alert)
            .bind(&info.remark)
            .bind(info.sum)
            .bind(info.cur)
            .bind(info.gps_type)
            .bind(info.status)
            .bind(&info.code)
            .execute(pool)
            .await?;

        Ok(true)
    }
}

pub async fn storage_del(code: &str, pool: &PgPool) -> Result<bool, Error> {
    sqlx::query("UPDATE public.ebike_storage SET update_date = $1, delete = $2 WHERE code = $3")
        .bind(Local::now().naive_local())
        .bind(true)
        .bind(code)
        .execute(pool)
        .await?;

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
        "SELECT * FROM public.ebike_storage WHERE delete = false{}{}{}",
        c, p, s
    ))
    .fetch_all(pool)
    .await?;

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

async fn storage_add_history(info: &StorageInfo, pool: &PgPool) -> Result<bool, Error> {
    sqlx::query(
        &format!("INSERT INTO public.ebike_storage_history_{}_{} VALUES ('{}', '{}', '{}', '{}', '{}', '{}', '{}', {}, '{}', '{}', {}, {}, '{}', '{}')", 
        Local::now().year(),
        Local::now().month(),
        Uuid::new_v4().simple().encode_lower(&mut Uuid::encode_buffer()),
        info.code,
        info.status.unwrap_or(0_i64),
        info.provide.as_ref().unwrap_or(&("".to_string())),
        info.gps.as_ref().unwrap_or(&(json!({"lng": "", "lat": ""}))),
        Local::now().format("%Y-%m-%d %H:%M:%S"),
        Local::now().format("%Y-%m-%d %H:%M:%S"),
        false,
        info.alert.as_ref().unwrap_or(&("".to_string())),
        info.remark.as_ref().unwrap_or(&("".to_string())),
        info.sum.unwrap_or(0_i64),
        info.cur.unwrap_or(0_i64),
        info.points.as_ref().unwrap_or(&("".to_string())),
        info.gps_type.unwrap_or(0_i64),
    )).execute(pool).await?;

    Ok(true)
}

pub async fn storage_history(code: &str, pool: &PgPool) -> Result<Vec<StorageInfo>, Error> {
    let rows = sqlx::query_as::<_, StorageInfo>(&format!(
        "SELECT * FROM public.ebike_storage_history_{}_{} WHERE code = '{}' AND delete = false",
        Local::now().year(),
        Local::now().month(),
        code,
    ))
    .fetch_all(pool)
    .await?;

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
            sum: Some(0_i64),
            cur: Some(0_i64),
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
    let rows = sqlx::query_as::<_, OrderInfo>(&format!(
        "SELECT hash FROM public.ebike_order_{}_{} WHERE hash = '{}' AND delete = false",
        Local::now().year(),
        Local::now().month(),
        info.hash,
    ))
    .fetch_all(pool)
    .await?;

    if rows.len() <= 0 {
        let hash = format!(
            "{}",
            Uuid::new_v4()
                .simple()
                .encode_lower(&mut Uuid::encode_buffer())
        );

        sqlx::query(
            &format!("INSERT INTO public.ebike_order_{}_{} VALUES ('{}', {}, '{}', {}, '{}', '{}', '{}', '{}', {}, '{}', 
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
        sqlx::query(
            &format!("UPDATE public.ebike_order_{}_{} SET code = '{}', status = {}, provide = {}, speed = {}, gps = '{}', 
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
    }
}

pub async fn order_query(
    code: &str,
    provide: &str,
    status: i64,
    time_start: &str,
    time_end: &str,
    order: &str,
    pay_status: i64,
    pay_type: i64,
    pay_time: &str,
    pool: &PgPool,
) -> Result<Vec<OrderInfo>, Error> {
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

    let mut t = "".to_string();
    if time_start != "" && time_end != "" {
        t = format!(
            " AND (time::json->'start')::TEXT LIKE '%{}%' AND (time::json->'end')::TEXT LIKE '%{}%'",
            time_start, time_end
        );
    } else if time_start != "" {
        t = format!(" AND (time::json->'start')::TEXT LIKE '%{}%'", time_start);
    } else if time_end != "" {
        t = format!(" AND (time::json->'end')::TEXT LIKE '%{}%'", time_end);
    }

    let mut o = "".to_string();
    if order != "".to_string() {
        o = format!(" AND order = '{}'", order);
    }

    let mut ps = "".to_string();
    if pay_status != -1 {
        ps = format!(" AND paystatus = '{}'", pay_status);
    }

    let mut payt = "".to_string();
    if pay_type != -1 {
        payt = format!(" AND paytype = '{}'", pay_type);
    }

    let mut pt = "".to_string();
    if pay_time != "" {
        pt = format!(" AND paytime LIKE '%{}%'", pay_time);
    }

    let rows = sqlx::query_as::<_, OrderInfo>(&format!(
        r#"SELECT * FROM public.ebike_order_{}_{} WHERE delete = false{}{}{}{}{}{}{}{}"#,
        &Local::now().year(),
        &Local::now().month(),
        c,
        p,
        s,
        t,
        o,
        ps,
        payt,
        pt
    ))
    .fetch_all(pool)
    .await?;

    if rows.len() <= 0 {
        Ok(vec![OrderInfo {
            hash: "".to_string(),
            code: Some("".to_string()),
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
            order: Some("".to_string()),
            payable: Some(0_f64),
            pay: Some(0_f64),
            coupon: Some(0_f64),
            refund: Some(0_f64),
            pay_status: Some(0_i64),
            pay_type: Some(0_i64),
            pay_time: Some(Local::now().naive_local()),
        }])
    } else {
        Ok(rows)
    }
}
