use actix_web::web;
use serde_json;
use serde_json::Value;
use sha2::{Digest, Sha256};

pub fn check_token(req: &web::Json<Value>) -> bool {
    match req["method"].as_str().unwrap() {
        "options" => {
            let orgin = format!("method={}", req["method"].as_str().unwrap().to_string(),);

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "car_add" => {
            let orgin = format!(
                "alert={}&code={}&gps={{lat={}&lng={}}}&method={}&provide={}&remark={}&speed={}&status={}&time={{end={}&start={}}}&type={}",
                req["alert"].as_str().unwrap().to_string(),
                req["code"].as_str().unwrap().to_string(),
                req["gps"]["lat"].as_str().unwrap().to_string(),
                req["gps"]["lng"].as_str().unwrap().to_string(),
                req["method"].as_str().unwrap().to_string(),
                req["provide"].as_str().unwrap().to_string(),
                req["remark"].as_str().unwrap().to_string(),
                req["speed"].as_f64().unwrap(),
                req["status"].as_i64().unwrap(),
                req["time"]["end"].as_str().unwrap().to_string(),
                req["time"]["start"].as_str().unwrap().to_string(),
                req["type"].as_i64().unwrap()
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "car_delete" => {
            let orgin = format!(
                "code={}&method={}",
                req["code"].as_str().unwrap(),
                req["method"].as_str().unwrap().to_string(),
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "car_query_all" => {
            let orgin = format!(
                "code={}&method={}&provide={}&status={}&time={{end={}&start={}}}",
                req["code"].as_str().unwrap(),
                req["method"].as_str().unwrap().to_string(),
                req["provide"].as_str().unwrap().to_string(),
                req["status"].as_i64().unwrap().to_string(),
                req["time"]["end"].as_str().unwrap().to_string(),
                req["time"]["start"].as_str().unwrap().to_string(),
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "storage_add" => {
            let orgin = format!(
                "alert={}&code={}&cur={}&gps={{lat={}&lng={}}}&method={}&points={}&provide={}&remark={}&status={}&sum={}&type={}",
                req["alert"].as_str().unwrap().to_string(),
                req["code"].as_str().unwrap().to_string(),
                req["cur"].as_i64().unwrap(),
                req["gps"]["lat"].as_str().unwrap().to_string(),
                req["gps"]["lng"].as_str().unwrap().to_string(),
                req["method"].as_str().unwrap().to_string(),
                req["points"].as_str().unwrap().to_string(),
                req["provide"].as_str().unwrap().to_string(),
                req["remark"].as_str().unwrap().to_string(),
                req["status"].as_i64().unwrap(),
                req["sum"].as_i64().unwrap(),
                req["type"].as_i64().unwrap(),
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "storage_delete" => {
            let orgin = format!(
                "code={}&method={}",
                req["code"].as_str().unwrap(),
                req["method"].as_str().unwrap().to_string(),
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "storage_query_all" => {
            let orgin = format!(
                "code={}&method={}&provide={}&status={}",
                req["code"].as_str().unwrap(),
                req["method"].as_str().unwrap().to_string(),
                req["provide"].as_str().unwrap().to_string(),
                req["status"].as_i64().unwrap().to_string(),
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        "order_add" => {
            let orgin = format!(
                "alert={}&code={}&coupon={}&gps={{lat={}&lng={}}}&method={}&order={}&pay={}&pay_status={}&pay_time={}&pay_type={}\
                &payable={}&provide={}&refund={}&remark={}&speed={}&status={}&time={{end={}&start={}}}&type={}",
                req["alert"].as_str().unwrap().to_string(),
                req["code"].as_str().unwrap().to_string(),
                req["coupon"].as_f64().unwrap().to_string(),
                req["gps"]["lat"].as_str().unwrap().to_string(),
                req["gps"]["lng"].as_str().unwrap().to_string(),
                req["method"].as_str().unwrap().to_string(),
                req["order"].as_str().unwrap().to_string(),
                req["pay"].as_f64().unwrap().to_string(),
                req["pay_status"].as_i64().unwrap().to_string(),
                req["pay_time"].as_str().unwrap().to_string(),
                req["pay_type"].as_i64().unwrap().to_string(),
                req["payable"].as_f64().unwrap().to_string(),
                req["provide"].as_str().unwrap().to_string(),
                req["refund"].as_f64().unwrap().to_string(),
                req["remark"].as_str().unwrap().to_string(),
                req["speed"].as_f64().unwrap(),
                req["status"].as_i64().unwrap(),
                req["time"]["end"].as_str().unwrap().to_string(),
                req["time"]["start"].as_str().unwrap().to_string(),
                req["type"].as_i64().unwrap()
            );

            let mut hasher = Sha256::new();
            hasher.update(&orgin);
            let result = hasher.finalize();

            if req["token"].as_str().unwrap().to_string() == format!("{:x}", result) {
                return true;
            } else {
                return false;
            }
        }
        _ => return false,
    }
}
