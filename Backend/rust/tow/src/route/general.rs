use crate::db;
use crate::Conf;
use actix_files::NamedFile;
use actix_identity::Identity;
use actix_multipart::Multipart;
use actix_web::{get, post, web, Error, HttpMessage, HttpRequest, HttpResponse, Result};
use chrono::{Local, NaiveDateTime};
use futures::{StreamExt, TryStreamExt};
use serde_json::{json, Value};
use sqlx::PgPool;
use std::path::PathBuf;
use std::{
    collections::HashMap,
    io::{prelude::*, Write},
};
use uuid::Uuid;

#[get("/{filename:.*}")]
async fn index(req: HttpRequest) -> Result<NamedFile, Error> {
    let path: PathBuf;

    if req
        .match_info()
        .query("filename")
        .parse::<String>()
        .unwrap()
        == ""
    {
        path = "./web/index.html".into();
    } else {
        path = req.match_info().query("filename").parse().unwrap();
    }

    return Ok(NamedFile::open(path)?.use_last_modified(true));
}

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
            "msg": result.err().unwrap().to_string()
        })));
    }

    let result = result.unwrap();

    match Identity::login(&request.extensions(), result.clone().id) {
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
async fn loginout() -> Result<HttpResponse, Error> {
    // id.logout();
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
            "msg": result.err().unwrap().to_string()
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

#[post("/user")]
async fn user(
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

    let result = db::general::user_update(
        req["username"].as_str().unwrap_or(""),
        req["password"].as_str().unwrap_or(""),
        req["nickname"].as_str().unwrap_or(""),
        req["update_user"].as_str().unwrap_or(""),
        &pool,
    )
    .await
    .unwrap();

    Ok(HttpResponse::Ok().json(result))
}

fn date_check(date: &str) -> Option<NaiveDateTime> {
    if date != "1000-01-01 00:00:00" {
        let a = NaiveDateTime::parse_from_str(date, "%Y-%m-%d %H:%M:%S").unwrap_or(
            NaiveDateTime::parse_from_str("1000-01-01 00:00:00", "%Y-%m-%d %H:%M:%S").unwrap(),
        );

        if a != NaiveDateTime::parse_from_str("1000-01-01 00:00:00", "%Y-%m-%d %H:%M:%S").unwrap() {
            return Some(NaiveDateTime::parse_from_str(date, "%Y-%m-%d %H:%M:%S").unwrap());
        }
    }

    Some(NaiveDateTime::parse_from_str("1000-01-01 00:00:00", "%Y-%m-%d %H:%M:%S").unwrap())
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
        "count" => {
            let result = db::general::car_count(
                req["in_date"].as_str().unwrap_or(""),
                req["out_date"].as_str().unwrap_or(""),
                req["content"].as_str().unwrap_or(""),
                req["model"].as_str().unwrap_or(""),
                req["status"].as_str().unwrap_or(""),
                req["key"].as_str().unwrap_or(""),
                req["name"].as_str().unwrap_or(""),
                req["unit"].as_str().unwrap_or(""),
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().body(format!("{}", result)))
        }
        "query" => {
            let result = db::general::car_query(
                req["in_date"].as_str().unwrap_or(""),
                req["out_date"].as_str().unwrap_or(""),
                req["content"].as_str().unwrap_or(""),
                req["model"].as_str().unwrap_or(""),
                req["cur"].as_i64().unwrap_or(0_i64),
                req["max"].as_i64().unwrap_or(0_i64),
                req["sortBy"].as_str().unwrap_or(""),
                req["descending"].as_bool().unwrap_or(false),
                req["status"].as_str().unwrap_or(""),
                req["key"].as_str().unwrap_or(""),
                req["name"].as_str().unwrap_or(""),
                req["unit"].as_str().unwrap_or(""),
                &pool,
            )
            .await
            .unwrap();

            let mut res: String = r#"{ "value": ["#.to_string();
            for (n, v) in result.iter().enumerate() {
                res += &format!(r#"{}"#, serde_json::to_string(&v)?);

                if n != result.len() - 1 {
                    res += ",";
                }
            }
            res += "]}";
            Ok(HttpResponse::Ok().json(res))
        }
        "add" => {
            let result = db::general::car_add(
                &db::general::CarInfo {
                    id: req["id"].as_i64().unwrap_or(0_i64),
                    license: Some(req["license"].as_str().unwrap_or("").to_string()),
                    vehicle: Some(req["vehicle"].as_str().unwrap_or("").to_string()),
                    engine: Some(req["engine"].as_str().unwrap_or("").to_string()),
                    car_type: Some(req["car_type"].as_str().unwrap_or("").to_string()),
                    dc_type: Some(req["dc_type"].as_str().unwrap_or("").to_string()),
                    dc_causes: Some(req["dc_causes"].as_str().unwrap_or("").to_string()),
                    car_color: Some(req["car_color"].as_str().unwrap_or("").to_string()),
                    dc_date: date_check(req["dc_date"].as_str().unwrap_or("1000-01-01 00:00:00")),
                    dc_address: Some(req["dc_address"].as_str().unwrap_or("").to_string()),
                    dc_key: Some(req["dc_key"].as_str().unwrap_or("").to_string()),
                    dc_party_name: Some(req["dc_party_name"].as_str().unwrap_or("").to_string()),
                    dc_party_cardid: Some(
                        req["dc_party_cardid"].as_str().unwrap_or("").to_string(),
                    ),
                    dc_party_tel: Some(req["dc_party_tel"].as_str().unwrap_or("").to_string()),
                    p_name: Some(req["p_name"].as_str().unwrap_or("").to_string()),
                    p_id: Some(req["p_id"].as_str().unwrap_or("").to_string()),
                    dc_acc: Some(req["dc_acc"].as_str().unwrap_or("").to_string()),
                    dc_name: Some(req["dc_name"].as_str().unwrap_or("").to_string()),
                    dc_into_date: date_check(
                        req["dc_into_date"]
                            .as_str()
                            .unwrap_or("1000-01-01 00:00:00"),
                    ),
                    car_remark: Some(req["car_remark"].as_str().unwrap_or("").to_string()),
                    driver: Some(req["driver"].as_str().unwrap_or("").to_string()),
                    operator: Some(req["operator"].as_str().unwrap_or("").to_string()),
                    drag_km: Some(req["drag_km"].as_str().unwrap_or("").to_string()),
                    drag_unit: Some(req["drag_unit"].as_str().unwrap_or("").to_string()),
                    drag_money: Some(req["drag_money"].as_str().unwrap_or("").to_string()),
                    cmd_unit: Some(req["cmd_unit"].as_str().unwrap_or("").to_string()),
                    cmd_user: Some(req["cmd_user"].as_str().unwrap_or("").to_string()),
                    cv: Some(req["cv"].as_str().unwrap_or("").to_string()),
                    cv_acc: Some(req["cv_acc"].as_str().unwrap_or("").to_string()),
                    cv_name: Some(req["cv_name"].as_str().unwrap_or("").to_string()),
                    cv_date: date_check(req["cv_date"].as_str().unwrap_or("1000-01-01 00:00:00")),
                    cv_opinion: Some(req["cv_opinion"].as_str().unwrap_or("").to_string()),
                    tv: Some(req["tv"].as_str().unwrap_or("").to_string()),
                    tv_acc: Some(req["tv_acc"].as_str().unwrap_or("").to_string()),
                    tv_name: Some(req["tv_name"].as_str().unwrap_or("").to_string()),
                    tv_date: date_check(req["tv_date"].as_str().unwrap_or("1000-01-01 00:00:00")),
                    tv_opinion: Some(req["tv_opinion"].as_str().unwrap_or("").to_string()),
                    rc_name: Some(req["rc_name"].as_str().unwrap_or("").to_string()),
                    rc_idcard: Some(req["rc_idcard"].as_str().unwrap_or("").to_string()),
                    rc_tel: Some(req["rc_tel"].as_str().unwrap_or("").to_string()),
                    parking_date: date_check(
                        req["dc_date"].as_str().unwrap_or("1000-01-01 00:00:00"),
                    ),
                    parking_unit: Some(req["parking_unit"].as_str().unwrap_or("").to_string()),
                    parking_money: Some(req["parking_money"].as_str().unwrap_or("").to_string()),
                    parking_payable: Some(
                        req["parking_payable"].as_str().unwrap_or("").to_string(),
                    ),
                    parking_paidin: Some(req["parking_paidin"].as_str().unwrap_or("").to_string()),
                    remark: Some(req["remark"].as_str().unwrap_or("").to_string()),
                    rs_name: Some(req["rs_name"].as_str().unwrap_or("").to_string()),
                    rs_acc: Some(req["rs_acc"].as_str().unwrap_or("").to_string()),
                    rs_date: date_check(req["rs_date"].as_str().unwrap_or("1000-01-01 00:00:00")),
                    attachment: Some(req["attachment"].clone()),
                    create_date: Some(Local::now().naive_local()),
                    create_user: Some(req["create_user"].as_str().unwrap_or("").to_string()),
                    update_date: Some(Local::now().naive_local()),
                    update_user: Some(req["update_user"].as_str().unwrap_or("").to_string()),
                    delete: Some(req["delete"].as_bool().unwrap_or(false)),
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

#[post("/send")]
async fn send(
    _id: Option<Identity>,
    mut payload: Multipart,
    config: web::Data<Conf>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    std::fs::create_dir_all(&config.upload_path)?;

    let mut filename = "".to_string();

    while let Ok(Some(mut field)) = payload.try_next().await {
        let content_type = field.content_disposition();
        filename = format!(
            "{}-{}",
            Uuid::new_v4().as_simple().to_string(),
            content_type.unwrap().get_filename().unwrap()
        );
        let filepath = format!("{}/{}", &config.upload_path, filename);

        let mut f = web::block(|| std::fs::File::create(filepath))
            .await?
            .unwrap();

        while let Some(chunk) = field.next().await {
            let data = chunk.unwrap();
            f = web::block(move || f.write_all(&data).map(|_| f))
                .await?
                .unwrap();
        }
    }

    Ok(HttpResponse::Ok().body(filename))
}

#[post("/public")]
async fn public(req: web::Json<Value>, pool: web::Data<PgPool>) -> Result<HttpResponse, Error> {
    match req["method"].as_str().unwrap() {
        "count" => {
            let result = db::general::car_count(
                req["in_date"].as_str().unwrap_or(""),
                req["out_date"].as_str().unwrap_or(""),
                req["content"].as_str().unwrap_or(""),
                req["model"].as_str().unwrap_or(""),
                req["status"].as_str().unwrap_or(""),
                req["key"].as_str().unwrap_or(""),
                req["name"].as_str().unwrap_or(""),
                req["unit"].as_str().unwrap_or(""),
                &pool,
            )
            .await
            .unwrap();
            Ok(HttpResponse::Ok().body(format!("{}", result)))
        }
        "query" => {
            let result = db::general::car_query_one(req["plate"].as_str().unwrap_or(""), &pool)
                .await
                .unwrap();

            let mut res: String = r#"{ "value": ["#.to_string();
            for (n, v) in result.iter().enumerate() {
                res += &format!(r#"{}"#, serde_json::to_string(&v)?);

                if n != result.len() - 1 {
                    res += ",";
                }
            }
            res += "]}";
            Ok(HttpResponse::Ok().json(res))
        }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}

#[post("/proxy")]
async fn proxy(
    _id: Option<Identity>,
    req: web::Json<Value>,
    http_client: web::Data<reqwest::Client>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    match req["method"].as_str().unwrap() {
        "get" => {
            if req["accessToken"].as_str() != Option::None {
                let body = http_client
                    .get(req["url"].as_str().unwrap())
                    .header("accessToken", req["accessToken"].as_str().unwrap())
                    .send()
                    .await
                    .unwrap()
                    .text()
                    .await
                    .unwrap();
                Ok(HttpResponse::Ok().body(body))
            } else {
                let body = http_client
                    .get(req["url"].as_str().unwrap())
                    .send()
                    .await
                    .unwrap()
                    .text()
                    .await
                    .unwrap();
                Ok(HttpResponse::Ok().body(body))
            }
        }
        "post_json" => {
            let resp = req["content"].as_object().unwrap();
            let mut map = HashMap::new();
            for elem in resp {
                map.insert(elem.0, elem.1);
            }
            let body = http_client
                .post(req["url"].as_str().unwrap())
                .json(&map)
                .send()
                .await
                .unwrap()
                .text()
                .await
                .unwrap();
            Ok(HttpResponse::Ok().body(body))
        }
        "post_form" => {
            let resp = req["content"].as_object().unwrap();
            let mut map = HashMap::new();
            for elem in resp {
                map.insert(elem.0, elem.1);
            }
            let body = http_client
                .post(req["url"].as_str().unwrap())
                .form(&map)
                .send()
                .await
                .unwrap()
                .text()
                .await
                .unwrap();
            Ok(HttpResponse::Ok().body(body))
        }
        // "upload" => {
        //     let client = reqwest::blocking::Client::new();
        //     let form = reqwest::blocking::multipart::Form::new()
        //         .file("file", req["content"]["file"].as_str().unwrap())?;

        //     let forward_req_resp = web::block(move || {
        //         client
        //             .post(req["url"].as_str().unwrap())
        //             .multipart(form)
        //             .send()
        //     })
        //     .await?;

        //     Ok(HttpResponse::Ok().body(forward_req_resp.text().unwrap()))
        // }
        _ => Ok(HttpResponse::Ok().body("error")),
    }
}

#[post("/proxy_send")]
async fn proxy_send(
    _id: Option<Identity>,
    mut payload: Multipart,
    config: web::Data<Conf>,
    http_client: web::Data<reqwest::Client>,
) -> Result<HttpResponse, Error> {
    if let Some(_id) = Option::<Identity>::None {
        return Ok(HttpResponse::Ok().json(json!({
            "status": 1i32,
            "msg": "loginout"
        })));
    }

    std::fs::create_dir_all(&config.upload_path)?;

    let mut filename;
    let mut filepath = "".to_string();

    while let Ok(Some(mut field)) = payload.try_next().await {
        let content_type = field.content_disposition();
        filename = format!(
            "{}-{}",
            Uuid::new_v4().as_simple().to_string(),
            content_type.unwrap().get_filename().unwrap()
        );
        let path = format!("{}/{}", &config.upload_path, filename);
        filepath = path.clone();

        let mut f = web::block(move || std::fs::File::create(path))
            .await?
            .unwrap();

        while let Some(chunk) = field.next().await {
            let data = chunk.unwrap();
            f = web::block(move || f.write_all(&data).map(|_| f))
                .await?
                .unwrap();
        }
    }

    let mut reader = std::fs::File::open(filepath)?;
    let mut buffer = Vec::new();
    reader.read_to_end(&mut buffer)?;

    let form = reqwest::multipart::Form::new();
    let form = form.part("file", reqwest::multipart::Part::bytes(buffer));

    let body = http_client
        .post("http://183.224.146.21:9005/oamp-store/file/uploadImgFile")
        .multipart(form)
        .send()
        .await
        .unwrap()
        .text()
        .await
        .unwrap();

    Ok(HttpResponse::Ok().body(body))
}
