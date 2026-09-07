use crate::db;
use actix_identity::Identity;
use actix_web::get;
use actix_web::HttpMessage;
use actix_web::HttpRequest;
use actix_web::{post, web, Error, HttpResponse, Result};
use chrono::Local;
use serde_json::{json, Value};
use sqlx::postgres::PgPool;

#[post("/login")]
async fn login(
    request: HttpRequest,
    req: web::Json<Value>,
    pool: web::Data<PgPool>,
) -> Result<HttpResponse, Error> {
    let result = db::general::login(
        &req["username"].as_str().unwrap().to_string(),
        &req["password"].as_str().unwrap().to_string(),
        &pool,
    )
    .await;

    if result.is_err() {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "username or password error"
        })));
    }

    let result = result.unwrap();

    match Identity::login(&request.extensions(), result.id.to_string()) {
        Err(error) => Ok(HttpResponse::Ok().json(json!({
            "status": 0i32,
            "msg": format!("{:?}", error)
        }))),
        _ => Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": result
        }))),
    }
}

#[get("/loginout")]
async fn loginout(id: Identity) -> Result<HttpResponse, Error> {
    id.logout();
    Ok(HttpResponse::Ok().body("ok"))
}

#[get("/info/{id}")]
async fn info(
    request: HttpRequest,
    path: web::Path<String>,
    _id: Option<Identity>,
    pool: web::Data<PgPool>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    let result = db::general::user_info(path.into_inner(), &pool).await;

    if result.is_err() {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "id error"
        })));
    }

    let result = result.unwrap();

    match Identity::login(&request.extensions(), result.id.to_string()) {
        Err(error) => Ok(HttpResponse::Ok().json(json!({
            "status": 0i32,
            "msg": format!("{:?}", error)
        }))),
        _ => Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": result
        }))),
    }
}

#[post("/options")]
async fn options(
    req: web::Json<Value>,
    _id: Option<Identity>,
    pool: web::Data<PgPool>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    match req["method"].as_str().unwrap() {
        "query" => {
            let result = db::general::options_query(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "update" => {
            let result = db::general::options_update(
                &db::general::OptionsInfo {
                    name: req["name"].as_str().unwrap().to_string(),
                    options: json!({
                        "system": req["options"]["system"].as_i64().unwrap(),
                        "alert": req["options"]["system"].as_i64().unwrap(),
                    }),
                    level: req["level"].as_i64().unwrap_or(5_i64),
                    create_date: Some(Local::now().naive_local()),
                    update_date: Some(Local::now().naive_local()),
                },
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}

#[post("/car")]
async fn car(
    req: web::Json<Value>,
    _id: Option<Identity>,
    pool: web::Data<PgPool>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    match req["method"].as_str().unwrap() {
        "add" => {
            let result = db::general::car_add(
                &db::general::CarInfo {
                    hash: "".to_string(),
                    code: req["code"].as_str().unwrap().to_string(),
                    status: Some(req["status"].as_i64().unwrap_or(0_i64)),
                    provide: Some(req["provide"].as_str().unwrap().to_string()),
                    speed: Some(req["speed"].as_f64().unwrap()),
                    gps: Some(json!({
                        "lng": Some(req["gps"]["lng"].as_str().unwrap().to_string()),
                        "lat": Some(req["gps"]["lat"].as_str().unwrap().to_string()),
                    })),
                    gps_type: Some(req["gps_type"].as_i64().unwrap_or(0_i64)),
                    time: Some(json!({
                        "start": req["time"]["start"].as_str().unwrap().to_string(),
                        "end": req["time"]["end"].as_str().unwrap().to_string(),
                    })),
                    create_date: Some(Local::now().naive_local()),
                    update_date: Some(Local::now().naive_local()),
                    alert: Some(req["alert"].as_str().unwrap().to_string()),
                    remark: Some(req["remark"].as_str().unwrap().to_string()),
                },
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().body(format!("{}", result)))
        }
        "delete" => {
            let result = db::general::car_del(&req["code"].as_str().unwrap(), &pool)
                .await
                .unwrap();
            Ok(HttpResponse::Ok().body(format!("{}", result)))
        }
        "query" => {
            let result = db::general::car_query(
                &req["code"].as_str().unwrap(),
                &req["provide"].as_str().unwrap(),
                req["status"].as_i64().unwrap(),
                &req["time"]["start"].as_str().unwrap(),
                &req["time"]["end"].as_str().unwrap(),
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "history" => {
            let result = db::general::car_history(&req["code"].as_str().unwrap(), &pool)
                .await
                .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "alert" => {
            let result = db::general::car_alert(
                &req["code"].as_str().unwrap(),
                &req["provide"].as_str().unwrap(),
                req["status"].as_i64().unwrap(),
                &req["time"].as_str().unwrap(),
                &req["alert"].as_str().unwrap(),
                &req["remark"].as_str().unwrap(),
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}

#[post("/storage")]
async fn storage(
    req: web::Json<Value>,
    _id: Option<Identity>,
    pool: web::Data<PgPool>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    match req["method"].as_str().unwrap() {
        "add" => {
            let result = db::general::storage_add(
                &db::general::StorageInfo {
                    hash: "".to_string(),
                    code: req["code"].as_str().unwrap().to_string(),
                    provide: Some(req["provide"].as_str().unwrap().to_string()),
                    status: Some(req["status"].as_i64().unwrap_or(0_i64)),
                    sum: Some(req["sum"].as_i64().unwrap()),
                    cur: Some(req["cur"].as_i64().unwrap()),
                    gps: Some(json!({
                        "lng": req["gps"]["lng"].as_str().unwrap().to_string(),
                        "lat": req["gps"]["lat"].as_str().unwrap().to_string(),
                    })),
                    gps_type: Some(req["gps_type"].as_i64().unwrap_or(0_i64)),
                    create_date: Some(Local::now().naive_local()),
                    update_date: Some(Local::now().naive_local()),
                    alert: Some(req["alert"].as_str().unwrap().to_string()),
                    remark: Some(req["remark"].as_str().unwrap().to_string()),
                    points: Some(req["points"].as_str().unwrap().to_string()),
                },
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().body(format!("{}", result)))
        }
        "delete" => {
            let result = db::general::storage_del(&req["code"].as_str().unwrap(), &pool)
                .await
                .unwrap();
            Ok(HttpResponse::Ok().body(format!("{}", result)))
        }
        "query" => {
            let result = db::general::storage_query(
                &req["code"].as_str().unwrap(),
                &req["provide"].as_str().unwrap(),
                req["status"].as_i64().unwrap(),
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "history" => {
            let result = db::general::storage_history(&req["code"].as_str().unwrap(), &pool)
                .await
                .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}

#[post("/order")]
async fn order(
    req: web::Json<Value>,
    _id: Option<Identity>,
    pool: web::Data<PgPool>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    match req["method"].as_str().unwrap() {
        "query" => {
            let result = db::general::order_query(
                &req["code"].as_str().unwrap(),
                &req["provide"].as_str().unwrap(),
                req["status"].as_i64().unwrap(),
                &req["time"]["start"].as_str().unwrap(),
                &req["time"]["end"].as_str().unwrap(),
                &req["order"].as_str().unwrap(),
                req["paystatus"].as_i64().unwrap(),
                req["paytype"].as_i64().unwrap(),
                req["paytime"].as_str().unwrap(),
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "add" => {
            let mut t = req["pay_time"].as_str().unwrap();
            if t == "" {
                t = "2000-01-01 00:00:00";
            }
            let dt = chrono::NaiveDateTime::parse_from_str(t, "%Y-%m-%d %H:%M:%S");

            let result = db::general::order_add(
                &db::general::OrderInfo {
                    hash: "".to_string(),
                    code: Some(req["code"].as_str().unwrap().to_string()),
                    status: Some(req["status"].as_i64().unwrap_or(0_i64)),
                    provide: Some(req["provide"].as_str().unwrap().to_string()),
                    speed: Some(req["speed"].as_f64().unwrap()),
                    gps: Some(json!({
                        "lng": Some(req["gps"]["lng"].as_str().unwrap().to_string()),
                        "lat": Some(req["gps"]["lat"].as_str().unwrap().to_string()),
                    })),
                    gps_type: Some(req["gps_type"].as_i64().unwrap_or(0_i64)),
                    time: Some(json!({
                        "start": req["time"]["start"].as_str().unwrap().to_string(),
                        "end": req["time"]["end"].as_str().unwrap().to_string(),
                    })),
                    create_date: Some(Local::now().naive_local()),
                    update_date: Some(Local::now().naive_local()),
                    alert: Some(req["alert"].as_str().unwrap().to_string()),
                    remark: Some(req["remark"].as_str().unwrap().to_string()),
                    payable: Some(req["payable"].as_f64().unwrap()),
                    pay: Some(req["pay"].as_f64().unwrap()),
                    refund: Some(req["refund"].as_f64().unwrap()),
                    coupon: Some(req["coupon"].as_f64().unwrap()),
                    order: Some(req["order"].as_str().unwrap().to_string()),
                    pay_type: Some(req["pay_type"].as_i64().unwrap()),
                    pay_time: Some(dt.unwrap()),
                    pay_status: Some(req["pay_status"].as_i64().unwrap()),
                },
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().body(result))
        }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}
