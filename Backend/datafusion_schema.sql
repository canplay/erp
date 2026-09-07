DROP TABLE IF EXISTS "public"."account";
CREATE TABLE "public"."account" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "username" varchar(255) COLLATE "pg_catalog"."default",
  "password" varchar(255) COLLATE "pg_catalog"."default",
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "head" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "wechat" varchar(255) COLLATE "pg_catalog"."default",
  "alipay" varchar(255) COLLATE "pg_catalog"."default",
  "weibo" varchar(255) COLLATE "pg_catalog"."default",
  "email" varchar(255) COLLATE "pg_catalog"."default",
  "permissions" varchar(255) COLLATE "pg_catalog"."default",
  "last_login" timestamp(6),
  "last_ip" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "create_name" varchar(255) COLLATE "pg_catalog"."default",
  "update_name" varchar(255) COLLATE "pg_catalog"."default",
  "options" varchar(255) COLLATE "pg_catalog"."default",
  "password_orgin" varchar(255) COLLATE "pg_catalog"."default",
  "nickname" varchar(255) COLLATE "pg_catalog"."default",
  "sex" varchar(255) COLLATE "pg_catalog"."default",
  "province" varchar(255) COLLATE "pg_catalog"."default",
  "city" varchar(255) COLLATE "pg_catalog"."default",
  "country" varchar(255) COLLATE "pg_catalog"."default",
  "additive" varchar(4096) COLLATE "pg_catalog"."default",
  "idcard" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of account
DROP TABLE IF EXISTS "public"."chat_comments";
CREATE TABLE "public"."chat_comments" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "content" varchar(2048) COLLATE "pg_catalog"."default",
  "like" int4,
  "dislike" int4,
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "lng" varchar(255) COLLATE "pg_catalog"."default",
  "lat" varchar(255) COLLATE "pg_catalog"."default",
  "end_date" timestamp(6)
)
;

-- ----------------------------
-- Records of chat_comments
DROP TABLE IF EXISTS "public"."chat_comments_detail";
CREATE TABLE "public"."chat_comments_detail" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "content" varchar(2048) COLLATE "pg_catalog"."default",
  "parent" varchar(255) COLLATE "pg_catalog"."default",
  "like" int4,
  "dislike" int4,
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4
)
;

-- ----------------------------
-- Records of chat_comments_detail
DROP TABLE IF EXISTS "public"."clean_invoice";
CREATE TABLE "public"."clean_invoice" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "no" varchar(255) COLLATE "pg_catalog"."default",
  "imposing_no" int8,
  "imposing_name" varchar(255) COLLATE "pg_catalog"."default",
  "fingerprint" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" varchar(255) COLLATE "pg_catalog"."default",
  "zone" varchar(255) COLLATE "pg_catalog"."default",
  "payer" varchar(255) COLLATE "pg_catalog"."default",
  "sum" varchar(255) COLLATE "pg_catalog"."default",
  "sum_capital" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "review" varchar(255) COLLATE "pg_catalog"."default",
  "operator" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" varchar(255) COLLATE "pg_catalog"."default",
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "collection_name" varchar(255) COLLATE "pg_catalog"."default",
  "print" int8,
  "project" json,
  "delete" bool,
  "invalid" bool
)
;

-- ----------------------------
-- Records of clean_invoice
DROP TABLE IF EXISTS "public"."customer_supplier";
CREATE TABLE "public"."customer_supplier" (
  "id" uuid NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "representative" varchar(255) COLLATE "pg_catalog"."default",
  "address" varchar(4096) COLLATE "pg_catalog"."default",
  "capital" int4,
  "agent" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "class1" varchar(255) COLLATE "pg_catalog"."default",
  "class2" varchar(255) COLLATE "pg_catalog"."default",
  "upload" json,
  "type1" varchar(255) COLLATE "pg_catalog"."default",
  "type2" varchar(255) COLLATE "pg_catalog"."default",
  "score" int4,
  "level" varchar(255) COLLATE "pg_catalog"."default",
  "reward" varchar(255) COLLATE "pg_catalog"."default",
  "examine" varchar(4096) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" uuid,
  "create_date" timestamp(6),
  "update_user" uuid,
  "update_date" timestamp(6),
  "delete" int4
)
;

-- ----------------------------
-- Records of customer_supplier
DROP TABLE IF EXISTS "public"."ebike_car";
CREATE TABLE "public"."ebike_car" (
  "code" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "speed" float8,
  "gps" json,
  "create_date" timestamp(0),
  "update_date" timestamp(0),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "gps_type" int8,
  "time" json,
  "hash" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of ebike_car
DROP TABLE IF EXISTS "public"."ebike_car_history_2024_5";
CREATE TABLE "public"."ebike_car_history_2024_5" (
  "hash" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "speed" float8,
  "gps" json,
  "time" json,
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "gps_type" int8
)
;

-- ----------------------------
-- Records of ebike_car_history_2024_5
DROP TABLE IF EXISTS "public"."ebike_options";
CREATE TABLE "public"."ebike_options" (
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "options" json,
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "level" int8
)
;

-- ----------------------------
-- Records of ebike_options
DROP TABLE IF EXISTS "public"."ebike_order_2024_5";
CREATE TABLE "public"."ebike_order_2024_5" (
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "speed" float8,
  "gps" json,
  "time" json,
  "create_date" timestamp(0),
  "update_date" timestamp(0),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "gps_type" int8,
  "hash" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "payable" float8,
  "pay" float8,
  "refund" float8,
  "coupon" float8,
  "order" varchar(255) COLLATE "pg_catalog"."default",
  "paytype" int8,
  "paytime" timestamp(0),
  "paystatus" int8
)
;

-- ----------------------------
-- Records of ebike_order_2024_5
DROP TABLE IF EXISTS "public"."ebike_storage";
CREATE TABLE "public"."ebike_storage" (
  "hash" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "gps" json,
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "sum" int8,
  "cur" int8,
  "points" varchar(4096) COLLATE "pg_catalog"."default",
  "gps_type" int8
)
;

-- ----------------------------
-- Records of ebike_storage
DROP TABLE IF EXISTS "public"."ebike_storage_history_2024_5";
CREATE TABLE "public"."ebike_storage_history_2024_5" (
  "hash" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "gps" json,
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "sum" int8,
  "cur" int8,
  "points" varchar(4096) COLLATE "pg_catalog"."default",
  "gps_type" int8
)
;

-- ----------------------------
-- Records of ebike_storage_history_2024_5
DROP TABLE IF EXISTS "public"."ebike_user";
CREATE TABLE "public"."ebike_user" (
  "username" varchar(255) COLLATE "pg_catalog"."default",
  "password" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "status" int8,
  "last_login" timestamp(6),
  "level" int8,
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_user" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of ebike_user
DROP TABLE IF EXISTS "public"."funeral_goods";
CREATE TABLE "public"."funeral_goods" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "spec" varchar(255) COLLATE "pg_catalog"."default",
  "money" int8,
  "sum" int8,
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4,
  "create_name" varchar(255) COLLATE "pg_catalog"."default",
  "update_name" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of funeral_goods
DROP TABLE IF EXISTS "public"."funeral_order";
CREATE TABLE "public"."funeral_order" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "goods" varchar(255) COLLATE "pg_catalog"."default",
  "money" int8,
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4,
  "create_name" varchar(255) COLLATE "pg_catalog"."default",
  "update_name" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of funeral_order
DROP TABLE IF EXISTS "public"."groupbuy";
CREATE TABLE "public"."groupbuy" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "project" varchar(255) COLLATE "pg_catalog"."default",
  "company" varchar(255) COLLATE "pg_catalog"."default",
  "contact" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "items" text COLLATE "pg_catalog"."default",
  "amount" int4,
  "delivery" date,
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "remark" text COLLATE "pg_catalog"."default",
  "price_range" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4
)
;

-- ----------------------------
-- Records of groupbuy
DROP TABLE IF EXISTS "public"."gzcw_dcsb";
CREATE TABLE "public"."gzcw_dcsb" (
  "sb" varchar(255) COLLATE "pg_catalog"."default",
  "bw" varchar(255) COLLATE "pg_catalog"."default",
  "bh" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "zb" varchar(255) COLLATE "pg_catalog"."default",
  "bw_new" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6)
)
;

-- ----------------------------
-- Records of gzcw_dcsb
DROP TABLE IF EXISTS "public"."healthex";
CREATE TABLE "public"."healthex" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "sex" varchar(255) COLLATE "pg_catalog"."default",
  "idcard" varchar(255) COLLATE "pg_catalog"."default",
  "appointment" date,
  "age" varchar(255) COLLATE "pg_catalog"."default",
  "birthday" date,
  "marital" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "org1" varchar(255) COLLATE "pg_catalog"."default",
  "org2" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4
)
;

-- ----------------------------
-- Records of healthex
DROP TABLE IF EXISTS "public"."news";
CREATE TABLE "public"."news" (
  "id" int4 NOT NULL,
  "title" varchar(255) COLLATE "pg_catalog"."default",
  "date" timestamp(6),
  "author" varchar(255) COLLATE "pg_catalog"."default",
  "content" text COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4,
  "preview" varchar(255) COLLATE "pg_catalog"."default",
  "class" int4,
  "hot" int4
)
;

-- ----------------------------
-- Records of news
DROP TABLE IF EXISTS "public"."notify";
CREATE TABLE "public"."notify" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "topic" varchar(255) COLLATE "pg_catalog"."default",
  "subscriber" varchar(255) COLLATE "pg_catalog"."default",
  "msg" text COLLATE "pg_catalog"."default",
  "status" int4,
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "type" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of notify
DROP TABLE IF EXISTS "public"."options";
CREATE TABLE "public"."options" (
  "id" uuid NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "options" varchar(255) COLLATE "pg_catalog"."default",
  "level" int8,
  "create_date" timestamp(6),
  "create_user" uuid,
  "update_date" timestamp(6),
  "update_user" uuid,
  "delete" bool
)
;

-- ----------------------------
-- Records of options
DROP TABLE IF EXISTS "public"."pay";
CREATE TABLE "public"."pay" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "order" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "order_pay" text COLLATE "pg_catalog"."default",
  "amount" int4,
  "remark" text COLLATE "pg_catalog"."default",
  "create_service" varchar(255) COLLATE "pg_catalog"."default",
  "create_params" text COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_date" timestamp(6)
)
;

-- ----------------------------
-- Records of pay
DROP TABLE IF EXISTS "public"."permissions";
CREATE TABLE "public"."permissions" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "access" text COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4
)
;

-- ----------------------------
-- Records of permissions
DROP TABLE IF EXISTS "public"."project";
CREATE TABLE "public"."project" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "class" varchar(255) COLLATE "pg_catalog"."default",
  "license" varchar(255) COLLATE "pg_catalog"."default",
  "pledge" varchar(255) COLLATE "pg_catalog"."default",
  "cost" numeric(10,2),
  "deed" varchar(255) COLLATE "pg_catalog"."default",
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "purpose" varchar(255) COLLATE "pg_catalog"."default",
  "reply" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "area" varchar(255) COLLATE "pg_catalog"."default",
  "street" varchar(255) COLLATE "pg_catalog"."default",
  "location" varchar(255) COLLATE "pg_catalog"."default",
  "income" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "attachment" varchar(255) COLLATE "pg_catalog"."default",
  "public" int4,
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "preview" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of project
DROP TABLE IF EXISTS "public"."public_assets";
CREATE TABLE "public"."public_assets" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_name" varchar(255) COLLATE "pg_catalog"."default",
  "update_name" varchar(255) COLLATE "pg_catalog"."default",
  "history" text COLLATE "pg_catalog"."default",
  "locate" varchar(255) COLLATE "pg_catalog"."default",
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "locate_name" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of public_assets
DROP TABLE IF EXISTS "public"."public_assets_history";
CREATE TABLE "public"."public_assets_history" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "date_start" date,
  "date_end" date,
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_name" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" char(36) COLLATE "pg_catalog"."default",
  "update_name" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "remark" text COLLATE "pg_catalog"."default",
  "images" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of public_assets_history
DROP TABLE IF EXISTS "public"."public_info";
CREATE TABLE "public"."public_info" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "info" varchar(4096) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4
)
;

-- ----------------------------
-- Records of public_info
DROP TABLE IF EXISTS "public"."public_pricetable";
CREATE TABLE "public"."public_pricetable" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "title" varchar(255) COLLATE "pg_catalog"."default",
  "spec" varchar(255) COLLATE "pg_catalog"."default",
  "unit" varchar(255) COLLATE "pg_catalog"."default",
  "price" int8,
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4
)
;

-- ----------------------------
-- Records of public_pricetable
DROP TABLE IF EXISTS "public"."qdzy_news";
CREATE TABLE "public"."qdzy_news" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "title" varchar(255) COLLATE "pg_catalog"."default",
  "preview" varchar(255) COLLATE "pg_catalog"."default",
  "content" text COLLATE "pg_catalog"."default",
  "top" int4,
  "type" int4,
  "create_date" timestamp(0),
  "create_user" varchar COLLATE "pg_catalog"."default",
  "update_date" timestamp(0),
  "update_user" varchar COLLATE "pg_catalog"."default",
  "delete" int4,
  "slide" jsonb
)
;
COMMENT ON COLUMN "public"."qdzy_news"."top" IS '0 - 保留
1 - 置顶';
COMMENT ON COLUMN "public"."qdzy_news"."type" IS '0 - 保留
1 - 新闻
2 - 自有小区
3 - 新房
4 - 二手房
5 - 租房';

-- ----------------------------
-- Records of qdzy_news
DROP TABLE IF EXISTS "public"."qdzy_order";
CREATE TABLE "public"."qdzy_order" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "zone" varchar(255) COLLATE "pg_catalog"."default",
  "service" varchar(255) COLLATE "pg_catalog"."default",
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "address" text COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(0),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(0),
  "delete" int4,
  "remark" text COLLATE "pg_catalog"."default",
  "src" varchar(255) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."qdzy_order"."zone" IS '0 - 保留
1 - 西华云锦';
COMMENT ON COLUMN "public"."qdzy_order"."service" IS '0 - 保留
1 - 送水到家
2 - 家政服务
3 - 上门看房';

-- ----------------------------
-- Records of qdzy_order
DROP TABLE IF EXISTS "public"."qdzy_service";
CREATE TABLE "public"."qdzy_service" (
  "id" varchar COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "content" text COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar COLLATE "pg_catalog"."default",
  "delete" int4
)
;

-- ----------------------------
-- Records of qdzy_service
DROP TABLE IF EXISTS "public"."qdzy_zone";
CREATE TABLE "public"."qdzy_zone" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "content" text COLLATE "pg_catalog"."default",
  "create_date" timestamp(0),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(0),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4
)
;

-- ----------------------------
-- Records of qdzy_zone
DROP TABLE IF EXISTS "public"."repair_order";
CREATE TABLE "public"."repair_order" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "plate" varchar(255) COLLATE "pg_catalog"."default",
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "date_in" timestamp(6),
  "date_settle" timestamp(6),
  "class" varchar(255) COLLATE "pg_catalog"."default",
  "vin" varchar(255) COLLATE "pg_catalog"."default",
  "mileage" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "project" varchar(255) COLLATE "pg_catalog"."default",
  "parts" varchar(255) COLLATE "pg_catalog"."default",
  "cost_manhour" varchar(255) COLLATE "pg_catalog"."default",
  "cost_materials" varchar(255) COLLATE "pg_catalog"."default",
  "cost_other" varchar(255) COLLATE "pg_catalog"."default",
  "amount_total" float8,
  "amount_receipt" float8,
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4
)
;

-- ----------------------------
-- Records of repair_order
DROP TABLE IF EXISTS "public"."repair_storage";
CREATE TABLE "public"."repair_storage" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "tag" varchar(255) COLLATE "pg_catalog"."default",
  "num" int4,
  "unit" varchar(255) COLLATE "pg_catalog"."default",
  "price" float4,
  "date_in" timestamp(6),
  "date_out" timestamp(6),
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "img" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4
)
;

-- ----------------------------
-- Records of repair_storage
DROP TABLE IF EXISTS "public"."sellhouse";
CREATE TABLE "public"."sellhouse" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "title" varchar(255) COLLATE "pg_catalog"."default",
  "contact" varchar(255) COLLATE "pg_catalog"."default",
  "phone" varchar(255) COLLATE "pg_catalog"."default",
  "zone" varchar(255) COLLATE "pg_catalog"."default",
  "building" varchar(255) COLLATE "pg_catalog"."default",
  "room" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "preview" varchar(255) COLLATE "pg_catalog"."default",
  "price" varchar(255) COLLATE "pg_catalog"."default",
  "area" varchar(255) COLLATE "pg_catalog"."default",
  "amount" varchar(255) COLLATE "pg_catalog"."default",
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "delete" int4,
  "additional" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of sellhouse
DROP TABLE IF EXISTS "public"."service";
CREATE TABLE "public"."service" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "url" varchar(255) COLLATE "pg_catalog"."default",
  "client_id" varchar(255) COLLATE "pg_catalog"."default",
  "client_secret" varchar(255) COLLATE "pg_catalog"."default",
  "access_token" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of service
DROP TABLE IF EXISTS "public"."shop_item";
CREATE TABLE "public"."shop_item" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "title" varchar(255) COLLATE "pg_catalog"."default",
  "images" text COLLATE "pg_catalog"."default",
  "class" int4,
  "price" float8,
  "desc" text COLLATE "pg_catalog"."default",
  "num" int4,
  "delete" int4,
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of shop_item
DROP TABLE IF EXISTS "public"."sms";
CREATE TABLE "public"."sms" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "to" varchar(255) COLLATE "pg_catalog"."default",
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "msg" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Records of sms
DROP TABLE IF EXISTS "public"."ticket_seats";
CREATE TABLE "public"."ticket_seats" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "title" varchar(255) COLLATE "pg_catalog"."default",
  "num" int4,
  "seats_start" varchar(255) COLLATE "pg_catalog"."default",
  "seats_end" varchar(255) COLLATE "pg_catalog"."default",
  "date_start" timestamp(6),
  "date_end" timestamp(6),
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4,
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "qrcode" varchar(255) COLLATE "pg_catalog"."default",
  "address" varchar(255) COLLATE "pg_catalog"."default",
  "seats_mid" varchar(255) COLLATE "pg_catalog"."default",
  "owner" varchar(255) COLLATE "pg_catalog"."default",
  "price" varchar(255) COLLATE "pg_catalog"."default",
  "desc" varchar(255) COLLATE "pg_catalog"."default",
  "verify_user" varchar(255) COLLATE "pg_catalog"."default",
  "verify_date" timestamp(6),
  "verify_amount" varchar(255) COLLATE "pg_catalog"."default",
  "num_available" varchar(255) COLLATE "pg_catalog"."default",
  "num_limit" varchar(255) COLLATE "pg_catalog"."default",
  "order" varchar(255) COLLATE "pg_catalog"."default",
  "pay_need" bool
)
;

-- ----------------------------
-- Records of ticket_seats
DROP TABLE IF EXISTS "public"."token";
CREATE TABLE "public"."token" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "type" varchar(255) COLLATE "pg_catalog"."default",
  "data" text COLLATE "pg_catalog"."default",
  "date" timestamp(6)
)
;

-- ----------------------------
-- Records of token
DROP TABLE IF EXISTS "public"."tow_car";
CREATE TABLE "public"."tow_car" (
  "license" varchar(255) COLLATE "pg_catalog"."default",
  "vehicle" varchar(255) COLLATE "pg_catalog"."default",
  "engine" varchar(255) COLLATE "pg_catalog"."default",
  "car_type" varchar(255) COLLATE "pg_catalog"."default",
  "dc_type" varchar(255) COLLATE "pg_catalog"."default",
  "dc_causes" varchar(255) COLLATE "pg_catalog"."default",
  "car_color" varchar(255) COLLATE "pg_catalog"."default",
  "dc_date" timestamp(6),
  "dc_address" varchar(255) COLLATE "pg_catalog"."default",
  "dc_key" varchar(255) COLLATE "pg_catalog"."default",
  "dc_party_name" varchar(255) COLLATE "pg_catalog"."default",
  "dc_party_cardid" varchar(255) COLLATE "pg_catalog"."default",
  "dc_party_tel" varchar(255) COLLATE "pg_catalog"."default",
  "p_name" varchar(255) COLLATE "pg_catalog"."default",
  "p_id" varchar(255) COLLATE "pg_catalog"."default",
  "dc_name" varchar(255) COLLATE "pg_catalog"."default",
  "dc_acc" varchar(255) COLLATE "pg_catalog"."default",
  "dc_into_date" timestamp(6),
  "car_remark" varchar(255) COLLATE "pg_catalog"."default",
  "driver" varchar(255) COLLATE "pg_catalog"."default",
  "operator" varchar(255) COLLATE "pg_catalog"."default",
  "drag_km" varchar(255) COLLATE "pg_catalog"."default",
  "drag_unit" varchar(255) COLLATE "pg_catalog"."default",
  "drag_money" varchar(255) COLLATE "pg_catalog"."default",
  "cmd_unit" varchar(255) COLLATE "pg_catalog"."default",
  "cmd_user" varchar(255) COLLATE "pg_catalog"."default",
  "cv" varchar(255) COLLATE "pg_catalog"."default",
  "cv_acc" varchar(255) COLLATE "pg_catalog"."default",
  "cv_name" varchar(255) COLLATE "pg_catalog"."default",
  "cv_date" timestamp(6),
  "cv_opinion" varchar(255) COLLATE "pg_catalog"."default",
  "tv" varchar(255) COLLATE "pg_catalog"."default",
  "tv_acc" varchar(255) COLLATE "pg_catalog"."default",
  "tv_name" varchar(255) COLLATE "pg_catalog"."default",
  "tv_date" timestamp(6),
  "tv_opinion" varchar(255) COLLATE "pg_catalog"."default",
  "rc_name" varchar(255) COLLATE "pg_catalog"."default",
  "rc_idcard" varchar(255) COLLATE "pg_catalog"."default",
  "rc_tel" varchar(255) COLLATE "pg_catalog"."default",
  "parking_date" timestamp(6),
  "parking_unit" varchar(255) COLLATE "pg_catalog"."default",
  "parking_money" varchar(255) COLLATE "pg_catalog"."default",
  "parking_payable" varchar(255) COLLATE "pg_catalog"."default",
  "parking_paidin" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "attachment" json,
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "id" int8 NOT NULL DEFAULT nextval('car_id_seq'::regclass),
  "rs_name" varchar(255) COLLATE "pg_catalog"."default",
  "rs_acc" varchar(255) COLLATE "pg_catalog"."default",
  "rs_date" timestamp(6),
  "delete" bool,
  "type" int4
)
;

-- ----------------------------
-- Records of tow_car
DROP TABLE IF EXISTS "public"."tow_car_class";
CREATE TABLE "public"."tow_car_class" (
  "id" int8 NOT NULL,
  "cpt" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "free_time" int8,
  "hm10" varchar(255) COLLATE "pg_catalog"."default",
  "hm24" varchar(255) COLLATE "pg_catalog"."default",
  "gratis_day" int8,
  "cost_day" varchar(255) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."tow_car_class"."cpt" IS '名称';
COMMENT ON COLUMN "public"."tow_car_class"."remark" IS '描述';
COMMENT ON COLUMN "public"."tow_car_class"."free_time" IS '停车免费时间';
COMMENT ON COLUMN "public"."tow_car_class"."hm10" IS '10小时内收费';
COMMENT ON COLUMN "public"."tow_car_class"."hm24" IS '10-24小时内收费';
COMMENT ON COLUMN "public"."tow_car_class"."gratis_day" IS '免费天数';
COMMENT ON COLUMN "public"."tow_car_class"."cost_day" IS '超出后每天收费多少';

-- ----------------------------
-- Records of tow_car_class
DROP TABLE IF EXISTS "public"."tow_car_color";
CREATE TABLE "public"."tow_car_color" (
  "id" int8 NOT NULL,
  "cpt" varchar(10) COLLATE "pg_catalog"."default",
  "py" varchar(10) COLLATE "pg_catalog"."default",
  "index" int8
)
;

-- ----------------------------
-- Records of tow_car_color
DROP TABLE IF EXISTS "public"."tow_car_type";
CREATE TABLE "public"."tow_car_type" (
  "id" int8 NOT NULL,
  "type" varchar(30) COLLATE "pg_catalog"."default",
  "ccid" int8,
  "py" varchar(30) COLLATE "pg_catalog"."default",
  "cost" varchar(10) COLLATE "pg_catalog"."default",
  "index" int8,
  "dc_start_m" varchar(30) COLLATE "pg_catalog"."default",
  "dc_start_k" int8,
  "dc_bstart_m" varchar(30) COLLATE "pg_catalog"."default",
  "p_start_m" varchar(30) COLLATE "pg_catalog"."default",
  "p_start_h" int8,
  "p_bstart_m" varchar(30) COLLATE "pg_catalog"."default",
  "p_bstart_dm" varchar(30) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."tow_car_type"."type" IS '车辆类型';
COMMENT ON COLUMN "public"."tow_car_type"."ccid" IS '关联 tb_car_class  cc_id';
COMMENT ON COLUMN "public"."tow_car_type"."cost" IS '费用';
COMMENT ON COLUMN "public"."tow_car_type"."dc_start_m" IS '拖移起步价  多少钱';
COMMENT ON COLUMN "public"."tow_car_type"."dc_start_k" IS '拖移起步价  多少公里内';
COMMENT ON COLUMN "public"."tow_car_type"."dc_bstart_m" IS '拖移超过起步公里后  多少钱/公里';
COMMENT ON COLUMN "public"."tow_car_type"."p_start_m" IS '停车场起步价  多少元';
COMMENT ON COLUMN "public"."tow_car_type"."p_start_h" IS '停车场起步价  多少小时内';
COMMENT ON COLUMN "public"."tow_car_type"."p_bstart_m" IS '停车场超过起步价  多少元/小时';
COMMENT ON COLUMN "public"."tow_car_type"."p_bstart_dm" IS '停车场超过起步价  多少元/天';

-- ----------------------------
-- Records of tow_car_type
DROP TABLE IF EXISTS "public"."tow_dc_causes";
CREATE TABLE "public"."tow_dc_causes" (
  "id" int8 NOT NULL,
  "cpt" varchar(255) COLLATE "pg_catalog"."default",
  "index" int8,
  "dct_id" int8,
  "py" varchar(255) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."tow_dc_causes"."dct_id" IS '关联类别表唯一标识';

-- ----------------------------
-- Records of tow_dc_causes
DROP TABLE IF EXISTS "public"."tow_dc_type";
CREATE TABLE "public"."tow_dc_type" (
  "id" int8 NOT NULL,
  "cpt" varchar(255) COLLATE "pg_catalog"."default",
  "unit_id" varchar(255) COLLATE "pg_catalog"."default",
  "py" varchar(50) COLLATE "pg_catalog"."default",
  "index" int8
)
;

-- ----------------------------
-- Records of tow_dc_type
DROP TABLE IF EXISTS "public"."tow_options";
CREATE TABLE "public"."tow_options" (
  "id" uuid NOT NULL,
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "options" varchar(255) COLLATE "pg_catalog"."default",
  "level" int8,
  "create_date" timestamp(6),
  "create_user" uuid,
  "update_date" timestamp(6),
  "update_user" uuid,
  "delete" bool
)
;

-- ----------------------------
-- Records of tow_options
DROP TABLE IF EXISTS "public"."tow_tb_car";
CREATE TABLE "public"."tow_tb_car" (
  "c_id" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "c_license" varchar(20) COLLATE "pg_catalog"."default",
  "c_vehicle" varchar(30) COLLATE "pg_catalog"."default",
  "c_engine" varchar(30) COLLATE "pg_catalog"."default",
  "c_car_type" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_type" varchar(100) COLLATE "pg_catalog"."default",
  "c_dc_causes" varchar(255) COLLATE "pg_catalog"."default",
  "c_car_colour" varchar(20) COLLATE "pg_catalog"."default",
  "c_dc_date" varchar(20) COLLATE "pg_catalog"."default",
  "c_dc_date_str" varchar(20) COLLATE "pg_catalog"."default",
  "c_dc_time" varchar(20) COLLATE "pg_catalog"."default",
  "c_dc_time_str" varchar(20) COLLATE "pg_catalog"."default",
  "c_dc_dt" varchar(50) COLLATE "pg_catalog"."default",
  "c_dc_address" varchar(255) COLLATE "pg_catalog"."default",
  "c_dc_key" varchar(6) COLLATE "pg_catalog"."default",
  "c_dc_party_name" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_party_cardid" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_party_tel" varchar(20) COLLATE "pg_catalog"."default",
  "c_p_name" varchar(30) COLLATE "pg_catalog"."default",
  "c_p_id" varchar(50) COLLATE "pg_catalog"."default",
  "c_dc_name" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_acc" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_into_date" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_into_time" varchar(30) COLLATE "pg_catalog"."default",
  "c_dc_datetime" varchar(30) COLLATE "pg_catalog"."default",
  "c_remark" varchar(255) COLLATE "pg_catalog"."default",
  "c_driver" varchar(20) COLLATE "pg_catalog"."default",
  "c_operator" varchar(20) COLLATE "pg_catalog"."default",
  "c_Drag_Km" int8,
  "c_cmd_unit" varchar(255) COLLATE "pg_catalog"."default",
  "c_cmd_user" varchar(20) COLLATE "pg_catalog"."default",
  "c_cv" varchar(20) COLLATE "pg_catalog"."default",
  "c_cv_acc" varchar(50) COLLATE "pg_catalog"."default",
  "c_cv_name" varchar(50) COLLATE "pg_catalog"."default",
  "c_cv_datetime" varchar(30) COLLATE "pg_catalog"."default",
  "c_cv_opinion" varchar(255) COLLATE "pg_catalog"."default",
  "c_tv" varchar(20) COLLATE "pg_catalog"."default",
  "c_tv_acc" varchar(50) COLLATE "pg_catalog"."default",
  "c_tv_name" varchar(50) COLLATE "pg_catalog"."default",
  "c_tv_datetime" varchar(30) COLLATE "pg_catalog"."default",
  "c_tv_opinion" varchar(255) COLLATE "pg_catalog"."default",
  "c_pic_id" varchar(50) COLLATE "pg_catalog"."default",
  "c_video_1" varchar(255) COLLATE "pg_catalog"."default",
  "c_video_2" varchar(255) COLLATE "pg_catalog"."default",
  "attachment" varchar(255) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."tow_tb_car"."c_license" IS '车牌';
COMMENT ON COLUMN "public"."tow_tb_car"."c_vehicle" IS '车架号';
COMMENT ON COLUMN "public"."tow_tb_car"."c_engine" IS '发动机号';
COMMENT ON COLUMN "public"."tow_tb_car"."c_car_type" IS '车辆类型';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_type" IS '扣车类型';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_causes" IS '扣车原因';
COMMENT ON COLUMN "public"."tow_tb_car"."c_car_colour" IS '颜色';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_date" IS '扣车日期';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_time" IS '扣车时间';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_dt" IS '扣车日期时间';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_key" IS '车钥匙（有无）';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_party_name" IS '当事人姓名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_party_cardid" IS '当事人身份证';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_party_tel" IS '当事人电话';
COMMENT ON COLUMN "public"."tow_tb_car"."c_p_name" IS '执勤人姓名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_p_id" IS '执勤人标识';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_name" IS '记录人姓名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_acc" IS '记录人登录名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_into_date" IS '进入停车场日期';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_into_time" IS '进入停车场时间';
COMMENT ON COLUMN "public"."tow_tb_car"."c_dc_datetime" IS '修改时间';
COMMENT ON COLUMN "public"."tow_tb_car"."c_driver" IS '拖车驾驶员';
COMMENT ON COLUMN "public"."tow_tb_car"."c_operator" IS '拖车操作员';
COMMENT ON COLUMN "public"."tow_tb_car"."c_Drag_Km" IS '拖移公里数';
COMMENT ON COLUMN "public"."tow_tb_car"."c_cmd_unit" IS '拖移车辆指令单位';
COMMENT ON COLUMN "public"."tow_tb_car"."c_cmd_user" IS '拖移车辆指令人';
COMMENT ON COLUMN "public"."tow_tb_car"."c_cv" IS '交通违法处理情况';
COMMENT ON COLUMN "public"."tow_tb_car"."c_cv_acc" IS '交警执行人登录名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_cv_name" IS '交警执行人姓名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_cv_opinion" IS '交警审批意见';
COMMENT ON COLUMN "public"."tow_tb_car"."c_tv" IS '城市违法处理情况';
COMMENT ON COLUMN "public"."tow_tb_car"."c_tv_acc" IS '城管执行人登录名';
COMMENT ON COLUMN "public"."tow_tb_car"."c_tv_name" IS '城管执行人名字';
COMMENT ON COLUMN "public"."tow_tb_car"."c_tv_opinion" IS '城管审批意见';
COMMENT ON COLUMN "public"."tow_tb_car"."c_pic_id" IS '图片关联编号';
COMMENT ON COLUMN "public"."tow_tb_car"."c_video_1" IS '视频路径1';
COMMENT ON COLUMN "public"."tow_tb_car"."c_video_2" IS '视频路径2';

-- ----------------------------
-- Records of tow_tb_car
DROP TABLE IF EXISTS "public"."tow_tb_release";
CREATE TABLE "public"."tow_tb_release" (
  "cr_id" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "cr_license" varchar(20) COLLATE "pg_catalog"."default",
  "cr_vehicle" varchar(30) COLLATE "pg_catalog"."default",
  "cr_engine" varchar(30) COLLATE "pg_catalog"."default",
  "cr_car_type" varchar(30) COLLATE "pg_catalog"."default",
  "cr_dc_type" varchar(100) COLLATE "pg_catalog"."default",
  "cr_dc_causes" varchar(255) COLLATE "pg_catalog"."default",
  "cr_car_colour" varchar(20) COLLATE "pg_catalog"."default",
  "cr_dc_date" varchar(20) COLLATE "pg_catalog"."default",
  "cr_dc_date_str" varchar(20) COLLATE "pg_catalog"."default",
  "cr_dc_time" varchar(20) COLLATE "pg_catalog"."default",
  "cr_dc_time_str" varchar(20) COLLATE "pg_catalog"."default",
  "cr_dc_dt" varchar(255) COLLATE "pg_catalog"."default",
  "cr_dc_address" varchar(255) COLLATE "pg_catalog"."default",
  "cr_dc_key" varchar(6) COLLATE "pg_catalog"."default",
  "cr_dc_party_name" varchar(30) COLLATE "pg_catalog"."default",
  "cr_dc_party_cardid" varchar(30) COLLATE "pg_catalog"."default",
  "cr_dc_party_tel" varchar(20) COLLATE "pg_catalog"."default",
  "cr_p_name" varchar(30) COLLATE "pg_catalog"."default",
  "cr_p_id" varchar(50) COLLATE "pg_catalog"."default",
  "cr_dc_name" varchar(30) COLLATE "pg_catalog"."default",
  "cr_dc_acc" varchar(50) COLLATE "pg_catalog"."default",
  "cr_dc_into_date" varchar(30) COLLATE "pg_catalog"."default",
  "cr_dc_into_time" varchar(30) COLLATE "pg_catalog"."default",
  "cr_dc_datetime" varchar(50) COLLATE "pg_catalog"."default",
  "cr_car_remark" varchar(255) COLLATE "pg_catalog"."default",
  "cr_driver" varchar(20) COLLATE "pg_catalog"."default",
  "cr_operator" varchar(20) COLLATE "pg_catalog"."default",
  "cr_Drag_Km" int8,
  "cr_Drag_unit" varchar(30) COLLATE "pg_catalog"."default",
  "cr_Drag_Money" varchar(30) COLLATE "pg_catalog"."default",
  "cr_cmd_unit" varchar(255) COLLATE "pg_catalog"."default",
  "cr_cmd_user" varchar(20) COLLATE "pg_catalog"."default",
  "cr_cv" varchar(20) COLLATE "pg_catalog"."default",
  "cr_cv_acc" varchar(50) COLLATE "pg_catalog"."default",
  "cr_cv_name" varchar(50) COLLATE "pg_catalog"."default",
  "cr_cv_datetime" varchar(30) COLLATE "pg_catalog"."default",
  "cr_cv_opinion" varchar(255) COLLATE "pg_catalog"."default",
  "cr_tv" varchar(20) COLLATE "pg_catalog"."default",
  "cr_tv_acc" varchar(50) COLLATE "pg_catalog"."default",
  "cr_tv_name" varchar(50) COLLATE "pg_catalog"."default",
  "cr_tv_datetime" varchar(30) COLLATE "pg_catalog"."default",
  "cr_tv_opinion" varchar(255) COLLATE "pg_catalog"."default",
  "cr_pic_id" varchar(50) COLLATE "pg_catalog"."default",
  "cr_video_1" varchar(255) COLLATE "pg_catalog"."default",
  "cr_video_2" varchar(255) COLLATE "pg_catalog"."default",
  "cr_rc_name" varchar(30) COLLATE "pg_catalog"."default",
  "cr_rc_idcard" varchar(30) COLLATE "pg_catalog"."default",
  "cr_rc_tel" varchar(30) COLLATE "pg_catalog"."default",
  "cr_parking_days" int8,
  "cr_parking_hour" int8,
  "cr_parking_minute" int8,
  "cr_parking_unit" varchar(255) COLLATE "pg_catalog"."default",
  "cr_parking_money" varchar(30) COLLATE "pg_catalog"."default",
  "cr_parking_payable" varchar(20) COLLATE "pg_catalog"."default",
  "cr_parking_paidin" varchar(20) COLLATE "pg_catalog"."default",
  "cr_remark" varchar(255) COLLATE "pg_catalog"."default",
  "cr_rs_name" varchar(30) COLLATE "pg_catalog"."default",
  "cr_rs_acc" varchar(50) COLLATE "pg_catalog"."default",
  "cr_rs_date" varchar(20) COLLATE "pg_catalog"."default",
  "cr_rs_time" varchar(20) COLLATE "pg_catalog"."default",
  "cr_rs_datetime" varchar(50) COLLATE "pg_catalog"."default",
  "attachment" varchar(255) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."tow_tb_release"."cr_dc_name" IS '扣车录入时的记录人姓名';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_Drag_Km" IS '拖移公里数';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_Drag_unit" IS '拖移单价(描述)';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_Drag_Money" IS '拖移费用';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_cmd_unit" IS '拖移车辆指令单位';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rc_name" IS '接车人姓名';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rc_idcard" IS '接车人身份证';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rc_tel" IS '接车人电话';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_parking_days" IS '停车天数 暂时不用';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_parking_hour" IS '停车多少小时';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_parking_unit" IS '停车单价（描述）';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_parking_money" IS '停车费';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_parking_payable" IS '应缴纳停车费';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_parking_paidin" IS '实缴停车费';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_remark" IS '放行意见';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rs_name" IS '放行人的姓名';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rs_acc" IS '放行人登录名';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rs_date" IS '放行日期';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rs_time" IS '放行时间';
COMMENT ON COLUMN "public"."tow_tb_release"."cr_rs_datetime" IS '放行时间';

-- ----------------------------
-- Records of tow_tb_release
DROP TABLE IF EXISTS "public"."work_order";
CREATE TABLE "public"."work_order" (
  "id" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "from" varchar(255) COLLATE "pg_catalog"."default",
  "to" varchar(255) COLLATE "pg_catalog"."default",
  "status" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "create_user" varchar(255) COLLATE "pg_catalog"."default",
  "create_id" varchar(255) COLLATE "pg_catalog"."default",
  "update_date" timestamp(6),
  "update_user" varchar(255) COLLATE "pg_catalog"."default",
  "update_id" varchar(255) COLLATE "pg_catalog"."default",
  "delete" int4
)
;

-- ----------------------------
-- Records of work_order
ALTER TABLE "public"."account" ADD CONSTRAINT "account_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."chat_comments" ADD CONSTRAINT "chat_comments_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."chat_comments_detail" ADD CONSTRAINT "chat_comments_copy1_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."clean_invoice" ADD CONSTRAINT "clean_invoice_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."customer_supplier" ADD CONSTRAINT "customer_supplier_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."ebike_car" ADD CONSTRAINT "ebike_car_pkey" PRIMARY KEY ("code");
ALTER TABLE "public"."ebike_car_history_2024_5" ADD CONSTRAINT "history_2021_6_pkey" PRIMARY KEY ("hash");
ALTER TABLE "public"."ebike_order_2024_5" ADD CONSTRAINT "car_copy1_pkey" PRIMARY KEY ("hash");
ALTER TABLE "public"."ebike_storage" ADD CONSTRAINT "ebike_storage_history_2024_5_copy1_pkey" PRIMARY KEY ("hash");
ALTER TABLE "public"."ebike_storage_history_2024_5" ADD CONSTRAINT "history_2021_6_copy1_pkey" PRIMARY KEY ("hash");
ALTER TABLE "public"."ebike_user" ADD CONSTRAINT "ebike_user_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."funeral_goods" ADD CONSTRAINT "funeral_goods_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."funeral_order" ADD CONSTRAINT "funeral_order_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."groupbuy" ADD CONSTRAINT "groupbuy_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."gzcw_dcsb" ADD CONSTRAINT "gzcw_dcsb_pkey" PRIMARY KEY ("bh");
ALTER TABLE "public"."healthex" ADD CONSTRAINT "healthex_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."news" ADD CONSTRAINT "news_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."notify" ADD CONSTRAINT "notify_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."options" ADD CONSTRAINT "options_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."pay" ADD CONSTRAINT "pay_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."permissions" ADD CONSTRAINT "permissions_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."project" ADD CONSTRAINT "project_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."public_assets" ADD CONSTRAINT "public_assets_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."public_assets_history" ADD CONSTRAINT "public_assets_history_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."public_info" ADD CONSTRAINT "public_info_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."public_pricetable" ADD CONSTRAINT "public_pricetable_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."qdzy_news" ADD CONSTRAINT "qdzy_news_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."qdzy_order" ADD CONSTRAINT "qdzy_order_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."qdzy_service" ADD CONSTRAINT "qdzy_service_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."qdzy_zone" ADD CONSTRAINT "qdzy_zone_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."repair_order" ADD CONSTRAINT "repair_order_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."repair_storage" ADD CONSTRAINT "repair_storage_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."sellhouse" ADD CONSTRAINT "sellhouse_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."service" ADD CONSTRAINT "service_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."shop_item" ADD CONSTRAINT "shop_item_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."sms" ADD CONSTRAINT "sms_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."ticket_seats" ADD CONSTRAINT "ticket_seats_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."token" ADD CONSTRAINT "token_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_car" ADD CONSTRAINT "car_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_car_class" ADD CONSTRAINT "tb_car_class_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_car_color" ADD CONSTRAINT "tb_colour_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_car_type" ADD CONSTRAINT "tb_car_type_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_dc_causes" ADD CONSTRAINT "tb_dc_causes_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_dc_type" ADD CONSTRAINT "tb_dc_type_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_options" ADD CONSTRAINT "options_copy1_pkey" PRIMARY KEY ("id");
ALTER TABLE "public"."tow_tb_car" ADD CONSTRAINT "tb_car_pkey" PRIMARY KEY ("c_id");
ALTER TABLE "public"."tow_tb_release" ADD CONSTRAINT "tb_release_pkey" PRIMARY KEY ("cr_id");
ALTER TABLE "public"."work_order" ADD CONSTRAINT "work_order_pkey" PRIMARY KEY ("id");
