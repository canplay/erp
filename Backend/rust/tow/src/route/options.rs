use crate::db;
use actix_identity::Identity;
use actix_web::{post, web, Error, HttpResponse, Result};
use serde_json::{json, Value};
use sqlx::postgres::PgPool;
use uuid::Uuid;

#[post("/options")]
async fn options(
    _id: Option<Identity>,
    req: web::Json<Value>,
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
            let result = db::options::options_query(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "update" => {
            let result = db::options::options_update(
                &db::options::OptionsInfo {
                    id: Uuid::new_v4(),
                    name: Some(req["name"].as_str().unwrap().to_string()),
                    options: Some(
                        json!({
                            "system": req["options"]["system"].as_i64().unwrap(),
                            "alert": req["options"]["system"].as_i64().unwrap(),
                        })
                        .to_string(),
                    ),
                    level: Some(req["level"].as_i64().unwrap_or(0_i64)),
                    create_date: Option::None,
                    create_user: Option::None,
                    update_date: Option::None,
                    update_user: Option::None,
                },
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "car_class" => {
            let result = db::options::car_class(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "update_car_class" => {
            let result = db::options::update_car_class(
                &db::options::CarClassInfo {
                    id: req["id"].as_i64().unwrap_or(0_i64),
                    cpt: req["cpt"].as_str().unwrap_or("").to_string(),
                    remark: req["remark"].as_str().unwrap_or("").to_string(),
                    free_time: req["free_time"].as_i64().unwrap_or(0_i64),
                    hm10: req["hm10"].as_str().unwrap_or("").to_string(),
                    hm24: req["hm24"].as_str().unwrap_or("").to_string(),
                    gratis_day: req["gratis_day"].as_i64().unwrap_or(0_i64),
                    cost_day: req["cost_day"].as_str().unwrap_or("").to_string(),
                },
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "car_type" => {
            let result = db::options::car_type(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "car_color" => {
            let result = db::options::car_color(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "causes" => {
            let result = db::options::dc_causes(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        "causes_type" => {
            let result = db::options::dc_type(&pool).await.unwrap();
            Ok(HttpResponse::Ok().json(result))
        }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}
