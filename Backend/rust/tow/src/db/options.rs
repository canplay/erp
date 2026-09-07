use chrono::NaiveDateTime;
use serde::{Deserialize, Serialize};
use sqlx::{Error, FromRow, PgPool};
use uuid::Uuid;

#[derive(PartialEq, Serialize, Deserialize, FromRow)]
pub struct Id {
    pub id: i64,
}

#[derive(PartialEq, Serialize, Deserialize, FromRow)]
pub struct OptionsInfo {
    pub id: Uuid,
    pub name: Option<String>,
    pub options: Option<String>,
    pub level: Option<i64>,
    pub create_date: Option<NaiveDateTime>,
    pub create_user: Option<Uuid>,
    pub update_date: Option<NaiveDateTime>,
    pub update_user: Option<Uuid>,
}

#[derive(PartialEq, Serialize, Deserialize, FromRow)]
pub struct CarClassInfo {
    pub id: i64,
    pub cpt: String,
    pub remark: String,
    pub free_time: i64,
    pub hm10: String,
    pub hm24: String,
    pub gratis_day: i64,
    pub cost_day: String,
}

#[derive(PartialEq, Serialize, Deserialize, FromRow)]
pub struct CarTypeInfo {
    pub id: Option<i64>,
    #[sqlx(rename = "type")]
    pub car_type: Option<String>,
    pub ccid: Option<i64>,
    pub py: Option<String>,
    pub cost: Option<String>,
    pub index: Option<i64>,
    pub dc_start_m: Option<String>,
    pub dc_start_k: Option<i64>,
    pub dc_bstart_m: Option<String>,
    pub p_start_m: Option<String>,
    pub p_start_h: Option<i64>,
    pub p_bstart_m: Option<String>,
    pub p_bstart_dm: Option<String>,
}

#[derive(PartialEq, Eq, Serialize, Deserialize, FromRow)]
pub struct CarColorInfo {
    pub id: i64,
    pub cpt: String,
    pub py: String,
    pub index: i64,
}

#[derive(PartialEq, Eq, Serialize, Deserialize, FromRow)]
pub struct CausesInfo {
    pub id: i64,
    pub cpt: String,
    pub index: i64,
    pub dct_id: i64,
    pub py: String,
}

#[derive(PartialEq, Eq, Serialize, Deserialize, FromRow)]
pub struct CausesTypeInfo {
    pub id: i64,
    pub cpt: String,
    pub unit_id: String,
    pub py: String,
    pub index: i64,
}

pub async fn options_query(pool: &PgPool) -> Result<Vec<OptionsInfo>, Error> {
    let rows = sqlx::query_as::<_, OptionsInfo>("SELECT id, name, options, level, create_date, create_user, update_date, update_user FROM tow_options")
        .fetch_all(pool)
    .await?;

    Ok(rows)
}

pub async fn options_update(info: &OptionsInfo, pool: &PgPool) -> Result<bool, Error> {
    let rows = sqlx::query_as::<_, Id>("SELECT id FROM tow_options WHERE name = $1")
        .bind(info.name.as_ref())
        .fetch_one(pool)
        .await?;

    if rows.id <= 0 {
        let row = sqlx::query("INSERT INTO tow_options (name, options, level, create_date, create_user, update_date, update_user) VALUES ($1, $2, $3, $4, $5, $6, $7)")
            .bind(info.name.as_ref())
            .bind(info.options.as_ref())
            .bind(info.level)
            .bind(info.create_date)
            .bind(info.create_user.as_ref())
            .bind(info.update_date)
            .bind(info.update_user.as_ref())
            .execute(pool)
            .await?;

        if row.rows_affected() > 0 {
            Ok(true)
        } else {
            Ok(false)
        }
    } else {
        let row = sqlx::query(
                "UPDATE tow_options SET name = $1, options = $2, update_date = $3, level = $4 WHERE name = $1"
            )
            .bind(info.name.as_ref())
            .bind(info.options.as_ref())
            .bind(info.update_date)
            .bind(info.level)
            .execute(pool)
            .await?;

        if row.rows_affected() > 0 {
            Ok(true)
        } else {
            Ok(false)
        }
    }
}

pub async fn car_class(pool: &PgPool) -> Result<Vec<CarClassInfo>, Error> {
    let rows = sqlx::query_as::<_, CarClassInfo>(
        "SELECT id, cpt, remark, free_time, hm10, hm24, gratis_day, cost_day FROM tow_car_class",
    )
    .fetch_all(pool)
    .await?;

    Ok(rows)
}

pub async fn update_car_class(info: &CarClassInfo, pool: &PgPool) -> Result<bool, Error> {
    let row = sqlx::query_as::<_, Id>("SELECT id FROM tow_car_class WHERE id = $1")
        .bind(info.id)
        .fetch_one(pool)
        .await?;

    if row.id <= 0 {
        Ok(false)
    } else {
        let row = sqlx::query(
            "UPDATE tow_car_class SET cpt = $1, remark = $2, free_time = $3, hm10 = $4, hm24 = $5, gratis_day = $6, cost_day = $7 WHERE id = $8"
        )
        .bind(&info.cpt)
        .bind(&info.remark)
        .bind(info.free_time)
        .bind(&info.hm10)
        .bind(&info.hm24)
        .bind(info.gratis_day)
        .bind(&info.cost_day)
        .bind(info.id)
        .execute(pool)
        .await?;

        if row.rows_affected() > 0 {
            Ok(true)
        } else {
            Ok(false)
        }
    }
}

pub async fn car_type(pool: &PgPool) -> Result<Vec<CarTypeInfo>, Error> {
    let rows = sqlx::query_as::<_, CarTypeInfo>(
        "SELECT id, type, ccid, py, cost, index, dc_start_m, dc_start_k, dc_bstart_m, 
            p_start_m, p_start_h, p_bstart_m, p_bstart_dm FROM tow_car_type",
    )
    .fetch_all(pool)
    .await?;

    Ok(rows)
}

pub async fn car_color(pool: &PgPool) -> Result<Vec<CarColorInfo>, Error> {
    let rows = sqlx::query_as::<_, CarColorInfo>("SELECT id, cpt, py, index FROM tow_car_color")
        .fetch_all(pool)
        .await?;

    Ok(rows)
}

pub async fn dc_causes(pool: &PgPool) -> Result<Vec<CausesInfo>, Error> {
    let rows =
        sqlx::query_as::<_, CausesInfo>("SELECT id, cpt, index, dct_id, py FROM tow_dc_causes")
            .fetch_all(pool)
            .await?;

    Ok(rows)
}

pub async fn dc_type(pool: &PgPool) -> Result<Vec<CausesTypeInfo>, Error> {
    let rows =
        sqlx::query_as::<_, CausesTypeInfo>("SELECT id, cpt, unit_id, py, index FROM tow_dc_type")
            .fetch_all(pool)
            .await?;

    Ok(rows)
}
