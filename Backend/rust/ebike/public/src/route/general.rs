use crate::db;
use crate::global;
use actix_web::{post, web, Error, HttpResponse, Result};
use chrono::Local;
use serde_json::{json, Value};
use sqlx::postgres::PgPool;

#[post("/public")]
async fn public(req: web::Json<Value>, pool: web::Data<PgPool>) -> Result<HttpResponse, Error> {
    match global::check_token(&req) {
        true => match req["method"].as_str().unwrap() {
            "test" => Ok(HttpResponse::Ok().body("test")),
            "options" => {
                let result = db::general::options_query(&pool).await.unwrap();
                Ok(HttpResponse::Ok().json(result))
            }
            "car_add" => {
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
                        gps_type: Some(req["type"].as_i64().unwrap_or(0_i64)),
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
            "car_query_all" => {
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
            "storage_query_all" => {
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
            "order_add" => {
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
                        gps_type: Some(req["type"].as_i64().unwrap_or(0_i64)),
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
        },
        false => Ok(HttpResponse::Ok().body("token error")),
    }
}
