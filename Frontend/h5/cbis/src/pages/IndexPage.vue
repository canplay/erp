<template>
  <q-page padding>
    <div class="row">
      <q-card class="col">
        <q-card-section class="text-bold text-h6">待办事项</q-card-section>
        <q-list>
          <q-item
            clickable
            v-for="item in backlog"
            :key="item.id"
            :to="item.url"
          >
            <q-item-section>{{ item.title }}</q-item-section>
            <q-item-section side top>
              <q-badge
                v-if="item.status === '完成'"
                color="positive"
                label="完成"
              />
              <q-badge v-else color="negative" :label="item.status" />
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="text-bold text-h6">工程项目</q-card-section>
        <q-list>
          <q-item
            clickable
            v-for="item in project"
            :key="item.id"
            :to="item.url"
          >
            <q-item-section>{{ item.title }}</q-item-section>
            <q-item-section side top>
              <q-badge
                v-if="item.status === '完成'"
                color="positive"
                label="完成"
              />
              <q-badge v-else color="negative" :label="item.status" />
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="text-bold text-h6">日程安排</q-card-section>
        <q-list>
          <q-item
            clickable
            v-for="item in schedule"
            :key="item.id"
            :to="item.url"
          >
            <q-item-section>{{ item.title }}</q-item-section>
            <q-item-section side top>
              <q-badge
                v-if="item.status === '完成'"
                color="positive"
                label="完成"
              />
              <q-badge v-else color="negative" :label="item.status" />
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="text-bold text-h6">知识库</q-card-section>
        <q-list>
          <q-item
            clickable
            v-for="item in knowledge"
            :key="item.id"
            :to="item.url"
          >
            <q-item-section>{{ item.title }}</q-item-section>
            <q-item-section side top>
              <q-badge
                v-if="item.status === '完成'"
                color="positive"
                label="完成"
              />
              <q-badge v-else color="negative" :label="item.status" />
              {{ item.date }}
            </q-item-section>
          </q-item>
        </q-list>
      </q-card>
    </div>

    <div style="height: 8px" />

    <div class="row">
      <q-card class="col">
        <q-card-section class="text-h6">公司要闻</q-card-section>
        <q-list>
          <q-item
            clickable
            v-for="item in notice"
            :key="item.id"
            :to="item.url"
          >
            <q-item-section avatar>
              <q-avatar>
                <img :src="item.img" />
              </q-avatar>
            </q-item-section>
            <q-item-section>{{ item.title }}</q-item-section>
            <q-item-section side>{{ item.date }}</q-item-section>
          </q-item>
        </q-list>
      </q-card>

      <div class="col-auto" style="width: 8px" />

      <q-card class="col">
        <q-card-section class="text-h6">行业新闻</q-card-section>
        <q-list>
          <q-item clickable v-for="item in news" :key="item.id" :to="item.url">
            <q-item-section avatar>
              <q-avatar>
                <img :src="item.img" />
              </q-avatar>
            </q-item-section>
            <q-item-section>{{ item.title }}</q-item-section>
            <q-item-section side>{{ item.date }}</q-item-section>
          </q-item>
        </q-list>
      </q-card>
    </div>

    <div style="height: 8px" />

    <q-card>
      <q-card-section class="row">
        <q-input class="col" v-model="search" label="快速搜索" />

        <div class="col-auto" style="width: 8px" />

        <q-btn class="col-1" color="primary" label="搜索" />
      </q-card-section>
    </q-card>

    <div style="height: 8px" />

    <q-card>
      <q-card-section>
        <div class="row">
          <q-btn
            class="col"
            color="primary"
            label="团队沟通"
            size="32px"
            href="https://chat.example.com"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="primary"
            label="视频会议"
            size="32px"
            href="https://meet.example.com/"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="primary"
            label="网盘"
            size="32px"
            href="https://chat.example.com"
          />

          <div class="col-auto" style="width: 8px" />

          <q-btn
            class="col"
            color="primary"
            label="可视化"
            size="32px"
            href="http://100.100.100.102:8938"
          />
        </div>
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { ref } from 'vue';

interface StatusItem {
  id: number;
  title: string;
  date: string;
  status: string;
  url: string;
}

interface NewsItem {
  id: number;
  title: string;
  img: string;
  date: string;
  url: string;
}

const search = ref('');

const backlog = ref<StatusItem[]>([]);
const project = ref<StatusItem[]>([]);
const schedule = ref<StatusItem[]>([]);
const knowledge = ref<StatusItem[]>([]);

const notice = ref<NewsItem[]>([]);
const news = ref<NewsItem[]>([]);

for (let index = 0; index < 5; index++) {
  const element: StatusItem = {
    id: index,
    title: '待办标题' + index,
    date: '2023-03-13',
    status: index % 2 === 0 ? '完成' : '待办',
    url: '',
  };
  backlog.value.push(element);
}

for (let index = 0; index < 5; index++) {
  const element: StatusItem = {
    id: index,
    title: '项目标题' + index,
    date: '2023-03-13',
    status: index % 2 === 0 ? '完成' : '进行中',
    url: '',
  };
  project.value.push(element);
}

for (let index = 0; index < 5; index++) {
  const element: StatusItem = {
    id: index,
    title: '日程标题' + index,
    date: '2023-03-13',
    status: index % 1 === 0 ? '完成' : '未到时',
    url: '',
  };
  schedule.value.push(element);
}

for (let index = 0; index < 5; index++) {
  const element: StatusItem = {
    id: index,
    title: '知识库标题' + index,
    date: '2023-03-13',
    status: index % 1 === 0 ? '完成' : '未读',
    url: '',
  };
  knowledge.value.push(element);
}

for (let index = 0; index < 5; index++) {
  const element: NewsItem = {
    id: index,
    title: '要闻标题' + index,
    img: 'https://t7.baidu.com/it/u=963301259,1982396977&fm=193&f=GIF',
    date: '2023-03-13',
    url: '',
  };
  notice.value.push(element);
}

for (let index = 0; index < 5; index++) {
  const element: NewsItem = {
    id: index,
    title: '新闻标题' + index,
    img: 'https://t7.baidu.com/it/u=963301259,1982396977&fm=193&f=GIF',
    date: '2023-03-13',
    url: '',
  };
  news.value.push(element);
}
</script>
