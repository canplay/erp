use chrono::prelude::*;
use serde::{Deserialize, Serialize};
use serde_json::Value;
use sqlx::{postgres::PgPool, Error, FromRow};

#[derive(PartialEq, Eq, Serialize, Deserialize, FromRow, Clone)]
pub struct UserInfo {
    pub id: String,
    pub username: Option<String>,
    pub password_orgin: Option<String>,
    pub nickname: Option<String>,
    pub create_date: Option<NaiveDateTime>,
    pub create_user: Option<String>,
    pub update_date: Option<NaiveDateTime>,
    pub update_user: Option<String>,
}

#[derive(PartialEq, Eq, Serialize, Deserialize, FromRow, Clone)]
pub struct CarCount {
    pub count: i64,
}

#[derive(PartialEq, Eq, Serialize, Deserialize, FromRow, Clone)]
pub struct CarInfo {
    pub id: i64,
    pub license: Option<String>,
    pub vehicle: Option<String>,
    pub engine: Option<String>,
    pub car_type: Option<String>,
    pub dc_type: Option<String>,
    pub dc_causes: Option<String>,
    pub car_color: Option<String>,
    pub dc_date: Option<NaiveDateTime>,
    pub dc_address: Option<String>,
    pub dc_key: Option<String>,
    pub dc_party_name: Option<String>,
    pub dc_party_cardid: Option<String>,
    pub dc_party_tel: Option<String>,
    pub p_name: Option<String>,
    pub p_id: Option<String>,
    pub dc_acc: Option<String>,
    pub dc_name: Option<String>,
    pub dc_into_date: Option<NaiveDateTime>,
    pub car_remark: Option<String>,
    pub driver: Option<String>,
    pub operator: Option<String>,
    pub drag_km: Option<String>,
    pub drag_unit: Option<String>,
    pub drag_money: Option<String>,
    pub cmd_unit: Option<String>,
    pub cmd_user: Option<String>,
    pub cv: Option<String>,
    pub cv_acc: Option<String>,
    pub cv_name: Option<String>,
    pub cv_date: Option<NaiveDateTime>,
    pub cv_opinion: Option<String>,
    pub tv: Option<String>,
    pub tv_acc: Option<String>,
    pub tv_name: Option<String>,
    pub tv_date: Option<NaiveDateTime>,
    pub tv_opinion: Option<String>,
    pub rc_name: Option<String>,
    pub rc_idcard: Option<String>,
    pub rc_tel: Option<String>,
    pub parking_date: Option<NaiveDateTime>,
    pub parking_unit: Option<String>,
    pub parking_money: Option<String>,
    pub parking_payable: Option<String>,
    pub parking_paidin: Option<String>,
    pub remark: Option<String>,
    pub rs_name: Option<String>,
    pub rs_acc: Option<String>,
    pub rs_date: Option<NaiveDateTime>,
    pub attachment: Option<Value>,
    pub create_date: Option<NaiveDateTime>,
    pub create_user: Option<String>,
    pub update_date: Option<NaiveDateTime>,
    pub update_user: Option<String>,
    pub delete: Option<bool>,
}

pub async fn login(username: &str, password: &str, pool: &PgPool) -> Result<UserInfo, Error> {
    let row = sqlx::query_as::<_, UserInfo>(
        r#"SELECT id, username, password_orgin, nickname, create_date, create_user, update_date, 
            update_user FROM account WHERE username = $1 AND password_orgin = $2"#,
    )
    .bind(username)
    .bind(password)
    .fetch_one(pool)
    .await?;

    Ok(row)
}

pub async fn user_info(id: String, pool: &PgPool) -> Result<UserInfo, Error> {
    let row = sqlx::query_as::<_, UserInfo>(
        r#"SELECT id, username, password_orgin, nickname, create_date, create_user, update_date, 
            update_user FROM account WHERE id = $1"#,
    )
    .bind(id)
    .fetch_one(pool)
    .await?;

    Ok(row)
}

pub async fn user_update(
    username: &str,
    password: &str,
    nickname: &str,
    update_user: &str,
    pool: &PgPool,
) -> Result<bool, Error> {
    let row = sqlx::query(
        r#"UPDATE account SET nickname = $3, password_orgin = $2, update_date = $4, 
            update_user = $5 WHERE username = $1 AND password_orgin = $2"#,
    )
    .bind(username)
    .bind(password)
    .bind(nickname)
    .bind(Local::now().naive_local())
    .bind(update_user)
    .execute(pool)
    .await?;

    if row.rows_affected() > 0 {
        Ok(true)
    } else {
        Ok(false)
    }
}

pub async fn car_count(
    in_date: &str,
    out_date: &str,
    content: &str,
    model: &str,
    status: &str,
    key: &str,
    name: &str,
    unit: &str,
    pool: &PgPool,
) -> Result<i64, Error> {
    let mut stmt = r#"SELECT COUNT(id) FROM tow_car WHERE delete = false"#.to_string();

    match status {
        "未放行" => {
            stmt = format!("{} AND (rs_date IS NULL OR rs_date::text = '')", stmt).to_string()
        }
        "已放行" => {
            stmt = format!("{} AND (rs_date IS NOT NULL OR rs_date::text != '')", stmt).to_string()
        }
        _ => (),
    }

    let mut date_in = vec![];
    if in_date != "" {
        let a = in_date.find(" - ");

        if a == Option::None {
            date_in.push(in_date);
        } else {
            let array: Vec<&str> = in_date.split(" - ").collect();
            date_in.push(array[0]);
            date_in.push(array[1]);
        }
    }

    let mut date_out = vec![];
    if out_date != "" {
        let b = out_date.find(" - ");

        if b == Option::None {
            date_out.push(out_date);
        } else {
            let array: Vec<&str> = out_date.split(" - ").collect();
            date_out.push(array[0]);
            date_out.push(array[1]);
        }
    }

    if date_in.len() == 1 && date_out.len() == 1 {
        stmt = format!(
            "{} AND (dc_date::text LIKE '{}%' OR rs_date::text LIKE '{}%')",
            stmt, date_in[0], date_out[0]
        );
    } else {
        if date_in.len() == 1 {
            stmt = format!("{} AND dc_date::text LIKE '{}%'", stmt, date_in[0]);
        } else if date_in.len() == 2 {
            stmt = format!(
                "{} AND dc_date between '{}' and '{}'",
                stmt, date_in[0], date_in[1]
            );
        }

        if date_out.len() == 1 {
            stmt = format!("{} AND rs_date::text LIKE '{}%'", stmt, date_out[0]);
        } else if date_out.len() == 2 {
            stmt = format!(
                "{} AND rs_date between '{}' and '{}'",
                stmt, date_out[0], date_out[1]
            );
        }
    }

    if name != "" {
        stmt = format!("{} AND dc_name LIKE '%{}%'", stmt, name);
    }

    if unit == "城管部门" {
        stmt = format!("{} AND (cmd_unit = '城管部门' OR cmd_unit = '卧龙执法点' OR cmd_unit = '开化执法点' OR cmd_unit = '新平执法点')", stmt);
    } else if unit != "" {
        stmt = format!("{} AND cmd_unit = '{}'", stmt, unit);
    }

    if key != "" && key != "全部" {
        stmt = format!("{} AND dc_key = '{}'", stmt, key).to_string();
    }

    match model {
        "车牌" => {
            if content != "" {
                stmt = format!("{} AND license LIKE '%{}%'", stmt, content);
            }
        }
        "车辆类型" => {
            if content != "" {
                stmt = format!("{} AND car_type LIKE '%{}%'", stmt, content);
            }
        }
        "车身颜色" => {
            if content != "" {
                stmt = format!("{} AND car_color LIKE '%{}%'", stmt, content);
            }
        }
        "车架号" => {
            if content != "" {
                stmt = format!("{} AND vehicle LIKE '%{}%'", stmt, content);
            }
        }
        "发动机号" => {
            if content != "" {
                stmt = format!("{} AND engine LIKE '%{}%'", stmt, content);
            }
        }
        "车辆备注" => {
            if content != "" {
                stmt = format!("{} AND car_remark LIKE '%{}%'", stmt, content);
            }
        }
        "拖移备注" => {
            if content != "" {
                stmt = format!("{} AND remark LIKE '%{}%'", stmt, content);
            }
        }
        _ => (),
    }

    let row = sqlx::query_as::<_, CarCount>(stmt.as_str())
        .fetch_one(pool)
        .await?;

    Ok(row.count)
}

pub async fn car_query(
    in_date: &str,
    out_date: &str,
    content: &str,
    model: &str,
    cur: i64,
    max: i64,
    sort_by: &str,
    descending: bool,
    status: &str,
    key: &str,
    name: &str,
    unit: &str,
    pool: &PgPool,
) -> Result<Vec<CarInfo>, Error> {
    let mut stmt = r#"SELECT id, license, vehicle, engine, car_type, dc_type, dc_causes, car_color, dc_date, dc_address, dc_key, 
            dc_party_name, dc_party_cardid, dc_party_tel, p_name, p_id, dc_acc, dc_name, dc_into_date, car_remark, driver, operator, drag_km, 
            drag_unit, drag_money, cmd_unit, cmd_user, cv, cv_acc, cv_name, cv_date, cv_opinion, tv, tv_acc, tv_name, tv_date, 
            tv_opinion, rc_name, rc_idcard, rc_tel, parking_date, parking_unit, parking_money, parking_payable, parking_paidin, 
            remark, rs_name, rs_acc, rs_date, attachment, create_date, create_user, update_date, update_user, delete 
            FROM tow_car WHERE delete = false"#.to_string();

    match status {
        "未放行" => {
            stmt = format!("{} AND (rs_date IS NULL OR rs_date::text = '' OR rs_date::text = '1000-01-01 00:00:00')", stmt).to_string()
        }
        "已放行" => {
            stmt = format!("{} AND (rs_date IS NOT NULL OR rs_date::text != '')", stmt).to_string()
        }
        _ => (),
    }

    let mut date_in = vec![];
    if in_date != "" {
        let a = in_date.find(" - ");

        if a == Option::None {
            date_in.push(in_date);
        } else {
            let array: Vec<&str> = in_date.split(" - ").collect();
            date_in.push(array[0]);
            date_in.push(array[1]);
        }
    }

    let mut date_out = vec![];
    if out_date != "" {
        let b = out_date.find(" - ");

        if b == Option::None {
            date_out.push(out_date);
        } else {
            let array: Vec<&str> = out_date.split(" - ").collect();
            date_out.push(array[0]);
            date_out.push(array[1]);
        }
    }

    if date_in.len() == 1 && date_out.len() == 1 {
        stmt = format!(
            "{} AND (dc_date::text LIKE '{}%' OR rs_date::text LIKE '{}%')",
            stmt, date_in[0], date_out[0]
        );
    } else {
        if date_in.len() == 1 {
            stmt = format!("{} AND dc_date::text LIKE '{}%'", stmt, date_in[0]);
        } else if date_in.len() == 2 {
            stmt = format!(
                "{} AND dc_date between '{}' and '{}'",
                stmt, date_in[0], date_in[1]
            );
        }

        if date_out.len() == 1 {
            stmt = format!("{} AND rs_date::text LIKE '{}%'", stmt, date_out[0]);
        } else if date_out.len() == 2 {
            stmt = format!(
                "{} AND rs_date between '{}' and '{}'",
                stmt, date_out[0], date_out[1]
            );
        }
    }

    match model {
        "车牌" => {
            if content != "" {
                stmt = format!("{} AND license LIKE '%{}%'", stmt, content);
            }
        }
        "车辆类型" => {
            if content != "" {
                stmt = format!("{} AND car_type LIKE '%{}%'", stmt, content);
            }
        }
        "车身颜色" => {
            if content != "" {
                stmt = format!("{} AND car_color LIKE '%{}%'", stmt, content);
            }
        }
        "车架号" => {
            if content != "" {
                stmt = format!("{} AND vehicle LIKE '%{}%'", stmt, content);
            }
        }
        "发动机号" => {
            if content != "" {
                stmt = format!("{} AND engine LIKE '%{}%'", stmt, content);
            }
        }
        "记录人" => {
            if content != "" {
                stmt = format!("{} AND dc_name LIKE '%{}%'", stmt, content);
            }
        }
        "车辆备注" => {
            if content != "" {
                stmt = format!("{} AND car_remark LIKE '%{}%'", stmt, content);
            }
        }
        "拖移备注" => {
            if content != "" {
                stmt = format!("{} AND remark LIKE '%{}%'", stmt, content);
            }
        }
        _ => (),
    }

    if name != "" {
        stmt = format!("{} AND dc_name LIKE '%{}%'", stmt, name);
    }

    if unit == "城管部门" {
        stmt = format!("{} AND (cmd_unit = '城管部门' OR cmd_unit = '卧龙执法点' OR cmd_unit = '开化执法点' OR cmd_unit = '新平执法点')", stmt);
    } else if unit != "" {
        stmt = format!("{} AND cmd_unit = '{}'", stmt, unit);
    }

    if key != "" && key != "全部" {
        stmt = format!("{} AND dc_key = '{}'", stmt, key).to_string();
    }

    let mut sort = "".to_string();
    if sort_by != "" && descending {
        sort = format!("ORDER BY {} ASC", sort_by);
    } else if sort_by != "" && !descending {
        sort = format!("ORDER BY {} DESC", sort_by);
    }

    stmt = format!("{} {} LIMIT {} OFFSET {}", stmt, sort, max, cur);

    let rows = sqlx::query_as::<_, CarInfo>(stmt.as_str())
        .fetch_all(pool)
        .await?;

    Ok(rows)
}

pub async fn car_add(info: &CarInfo, pool: &PgPool) -> Result<bool, Error> {
    if info.id == 0 {
        let stmt = format!(
            r#"INSERT INTO
                    tow_car
                (license, vehicle, engine, car_type, dc_type, dc_causes, car_color, dc_date, 
                        dc_address, dc_key, dc_party_name, dc_party_cardid, dc_party_tel, p_name, p_id, 
                        dc_name, dc_acc, dc_into_date, car_remark, driver, operator, drag_km, drag_unit, 
                        drag_money, cmd_unit, cmd_user, cv, cv_acc, cv_name, cv_date, cv_opinion, tv, 
                        tv_acc, tv_name, tv_date, tv_opinion, rc_name, rc_idcard, rc_tel, parking_date, 
                        parking_unit, parking_money, parking_payable, parking_paidin, remark, rs_name, 
                        rs_acc, rs_date, attachment, create_date, create_user, update_date, update_user, 
                        delete)
                VALUES ('{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', 
                    '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', 
                    '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', 
                    {})"#,
            info.license.as_ref().unwrap(),
            info.vehicle.as_ref().unwrap(),
            info.engine.as_ref().unwrap(),
            info.car_type.as_ref().unwrap(),
            info.dc_type.as_ref().unwrap(),
            info.dc_causes.as_ref().unwrap(),
            info.car_color.as_ref().unwrap(),
            info.dc_date.unwrap(),
            info.dc_address.as_ref().unwrap(),
            info.dc_key.as_ref().unwrap(),
            info.dc_party_name.as_ref().unwrap(),
            info.dc_party_cardid.as_ref().unwrap(),
            info.dc_party_tel.as_ref().unwrap(),
            info.p_name.as_ref().unwrap(),
            info.p_id.as_ref().unwrap(),
            info.dc_name.as_ref().unwrap(),
            info.dc_acc.as_ref().unwrap(),
            info.dc_into_date.unwrap(),
            info.car_remark.as_ref().unwrap(),
            info.driver.as_ref().unwrap(),
            info.operator.as_ref().unwrap(),
            info.drag_km.as_ref().unwrap(),
            info.drag_unit.as_ref().unwrap(),
            info.drag_money.as_ref().unwrap(),
            info.cmd_unit.as_ref().unwrap(),
            info.cmd_user.as_ref().unwrap(),
            info.cv.as_ref().unwrap(),
            info.cv_acc.as_ref().unwrap(),
            info.cv_name.as_ref().unwrap(),
            info.cv_date.unwrap(),
            info.cv_opinion.as_ref().unwrap(),
            info.tv.as_ref().unwrap(),
            info.tv_acc.as_ref().unwrap(),
            info.tv_name.as_ref().unwrap(),
            info.tv_date.unwrap(),
            info.tv_opinion.as_ref().unwrap(),
            info.rc_name.as_ref().unwrap(),
            info.rc_idcard.as_ref().unwrap(),
            info.rc_tel.as_ref().unwrap(),
            info.parking_date.unwrap(),
            info.parking_unit.as_ref().unwrap(),
            info.parking_money.as_ref().unwrap(),
            info.parking_payable.as_ref().unwrap(),
            info.parking_paidin.as_ref().unwrap(),
            info.remark.as_ref().unwrap(),
            info.rs_name.as_ref().unwrap(),
            info.rs_acc.as_ref().unwrap(),
            info.rs_date.unwrap(),
            info.attachment.as_ref().unwrap(),
            info.create_date.unwrap(),
            info.create_user.as_ref().unwrap(),
            info.update_date.unwrap(),
            info.update_user.as_ref().unwrap(),
            info.delete.unwrap(),
        );

        let rows = sqlx::query(&stmt).execute(pool).await?;

        if rows.rows_affected() > 0 {
            Ok(true)
        } else {
            Ok(false)
        }
    } else {
        let stmt = format!(
            r#"UPDATE
                    tow_car
                SET
                    license = '{}', vehicle = '{}', engine = '{}', car_type = '{}', dc_type = '{}', dc_causes = '{}', 
                    car_color = '{}', dc_date = '{}', dc_address = '{}', dc_key = '{}', dc_party_name = '{}', 
                    dc_party_cardid = '{}', dc_party_tel = '{}', p_name = '{}', p_id = '{}', dc_name = '{}', dc_acc = '{}', 
                    dc_into_date = '{}', car_remark = '{}', driver = '{}', operator = '{}', drag_km = '{}', drag_unit = '{}', 
                    drag_money = '{}', cmd_unit = '{}', cmd_user = '{}', cv = '{}', cv_acc = '{}', cv_name = '{}', 
                    cv_date = '{}', cv_opinion = '{}', tv = '{}', tv_acc = '{}', tv_name = '{}', tv_date = '{}', 
                    tv_opinion = '{}', rc_name = '{}', rc_idcard = '{}', rc_tel = '{}', parking_date = '{}', parking_unit = '{}', 
                    parking_money = '{}', parking_payable = '{}', parking_paidin = '{}', remark = '{}', rs_name = '{}', 
                    rs_acc = '{}', rs_date = '{}', attachment = '{}', update_date = '{}', update_user = '{}', 
                    delete = {} WHERE id = {}"#,
            info.license.as_ref().unwrap(),
            info.vehicle.as_ref().unwrap(),
            info.engine.as_ref().unwrap(),
            info.car_type.as_ref().unwrap(),
            info.dc_type.as_ref().unwrap(),
            info.dc_causes.as_ref().unwrap(),
            info.car_color.as_ref().unwrap(),
            info.dc_date.unwrap(),
            info.dc_address.as_ref().unwrap(),
            info.dc_key.as_ref().unwrap(),
            info.dc_party_name.as_ref().unwrap(),
            info.dc_party_cardid.as_ref().unwrap(),
            info.dc_party_tel.as_ref().unwrap(),
            info.p_name.as_ref().unwrap(),
            info.p_id.as_ref().unwrap(),
            info.dc_name.as_ref().unwrap(),
            info.dc_acc.as_ref().unwrap(),
            info.dc_into_date.unwrap(),
            info.car_remark.as_ref().unwrap(),
            info.driver.as_ref().unwrap(),
            info.operator.as_ref().unwrap(),
            info.drag_km.as_ref().unwrap(),
            info.drag_unit.as_ref().unwrap(),
            info.drag_money.as_ref().unwrap(),
            info.cmd_unit.as_ref().unwrap(),
            info.cmd_user.as_ref().unwrap(),
            info.cv.as_ref().unwrap(),
            info.cv_acc.as_ref().unwrap(),
            info.cv_name.as_ref().unwrap(),
            info.cv_date.unwrap(),
            info.cv_opinion.as_ref().unwrap(),
            info.tv.as_ref().unwrap(),
            info.tv_acc.as_ref().unwrap(),
            info.tv_name.as_ref().unwrap(),
            info.tv_date.unwrap(),
            info.tv_opinion.as_ref().unwrap(),
            info.rc_name.as_ref().unwrap(),
            info.rc_idcard.as_ref().unwrap(),
            info.rc_tel.as_ref().unwrap(),
            info.parking_date.unwrap(),
            info.parking_unit.as_ref().unwrap(),
            info.parking_money.as_ref().unwrap(),
            info.parking_payable.as_ref().unwrap(),
            info.parking_paidin.as_ref().unwrap(),
            info.remark.as_ref().unwrap(),
            info.rs_name.as_ref().unwrap(),
            info.rs_acc.as_ref().unwrap(),
            info.rs_date.unwrap(),
            info.attachment.as_ref().unwrap(),
            info.update_date.unwrap(),
            info.update_user.as_ref().unwrap(),
            info.delete.as_ref().unwrap(),
            info.id,
        );

        let rows = sqlx::query(&stmt).execute(pool).await?;

        if rows.rows_affected() > 0 {
            Ok(true)
        } else {
            Ok(false)
        }
    }
}

pub async fn car_query_one(plate: &str, pool: &PgPool) -> Result<Vec<CarInfo>, Error> {
    let mut stmt = r#"SELECT id, license, vehicle, engine, car_type, dc_type, dc_causes, car_color, dc_date, dc_address, dc_key, 
            dc_party_name, dc_party_cardid, dc_party_tel, p_name, p_id, dc_acc, dc_name, dc_into_date, car_remark, driver, operator, drag_km, 
            drag_unit, drag_money, cmd_unit, cmd_user, cv, cv_acc, cv_name, cv_date, cv_opinion, tv, tv_acc, tv_name, tv_date, 
            tv_opinion, rc_name, rc_idcard, rc_tel, parking_date, parking_unit, parking_money, parking_payable, parking_paidin, 
            remark, rs_name, rs_acc, rs_date, attachment, create_date, create_user, update_date, update_user, delete 
            FROM tow_car WHERE delete = false"#.to_string();

    stmt = format!(
        "{} AND license LIKE '%{}%' ORDER BY dc_date DESC",
        stmt, plate
    );

    let rows = sqlx::query_as::<_, CarInfo>(stmt.as_str())
        .fetch_all(pool)
        .await?;

    Ok(rows)
}
