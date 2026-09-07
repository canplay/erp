import { defineStore } from 'pinia';
import { ref } from 'vue';

export interface Team {
  name: string;
  score: number;
  logo: File | null;
  logoUrl: string | null;
}

export interface ScoreboardData {
  id: number;
  titleText1: string;
  titleText2: string;
  titleText3: string;
  homeTeam: Team;
  guestTeam: Team;
  currentTime: number;
  isRunning: boolean;
  isCountdown: boolean;
  displayScale: number;
  visible: boolean;
  timerInterval: number | null; // 添加定时器引用
}

// 定义用于localStorage的接口（不包含timerInterval）
export interface StoredScoreboardData {
  id: number;
  titleText1: string;
  titleText2: string;
  titleText3: string;
  homeTeam: Omit<Team, 'logo'> & { logo: null }; // logo不会被序列化，所以总是null
  guestTeam: Omit<Team, 'logo'> & { logo: null };
  currentTime: number;
  isRunning: boolean;
  isCountdown: boolean;
  displayScale: number;
  visible: boolean;
}

export interface StoredState {
  scoreboards: StoredScoreboardData[];
  activeScoreboardId: number;
}

export const useScoreboardStore = defineStore('scoreboard', () => {
  // State - 现在只存储一个记分牌的数据
  const scoreboards = ref<ScoreboardData[]>([]);
  const activeScoreboardId = ref(0); // 当前活跃的记分牌ID，始终为0

  // Computed - 基于当前记分牌的计算属性
  const formattedTime = () => {
    const current = getCurrentScoreboard();
    if (!current) return '00:00';
    const totalSeconds = Math.abs(current.currentTime);
    const mins = Math.floor(totalSeconds / 60);
    const secs = totalSeconds % 60;

    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
  };

  const clockStateText = () => {
    const current = getCurrentScoreboard();
    if (!current) return '停止';
    return current.isRunning ? '运行中' : '停止';
  };

  // Actions
  const createScoreboard = () => {
    const newId = 0; // 只有一个记分牌，ID固定为0

    const newScoreboard: ScoreboardData = {
      id: newId,
      titleText1: '',
      titleText2: '',
      titleText3: '',
      homeTeam: {
        name: '主队',
        score: 0,
        logo: null,
        logoUrl: null,
      },
      guestTeam: {
        name: '客队',
        score: 0,
        logo: null,
        logoUrl: null,
      },
      currentTime: 0,
      isRunning: false,
      isCountdown: true,
      displayScale: 1,
      visible: true,
      timerInterval: null, // 新记分牌的定时器初始为空
    };

    // 如果已经有记分牌了，就替换它
    if (scoreboards.value.length > 0) {
      scoreboards.value[0] = newScoreboard;
    } else {
      scoreboards.value.push(newScoreboard);
    }

    return newId;
  };

  const setActiveScoreboard = () => {
    // 先停止当前记分牌的定时器
    const current = getCurrentScoreboard();
    if (current && current.timerInterval !== null) {
      clearInterval(current.timerInterval);
      current.timerInterval = null;
    }

    // 由于只有一个记分牌，直接设置为0
    activeScoreboardId.value = 0;

    // 如果新记分牌之前是运行状态，则启动它的定时器
    const newActiveScoreboard = getCurrentScoreboard();
    if (newActiveScoreboard && newActiveScoreboard.isRunning) {
      startTimer();
    }
  };

  // 启动记分牌的定时器
  const startTimer = () => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    // 如果已有定时器，先清除
    if (scoreboard.timerInterval !== null) {
      clearInterval(scoreboard.timerInterval);
    }

    // 设置新的定时器
    scoreboard.timerInterval = window.setInterval(() => {
      if (scoreboard.isRunning) {
        if (scoreboard.isCountdown) {
          scoreboard.currentTime = Math.max(0, scoreboard.currentTime - 1);
          if (scoreboard.currentTime <= 0) {
            scoreboard.isRunning = false;
            if (scoreboard.timerInterval !== null) {
              clearInterval(scoreboard.timerInterval);
              scoreboard.timerInterval = null;
            }
          }
        } else {
          scoreboard.currentTime++;
        }
      }
    }, 1000);
  };

  // 停止记分牌的定时器
  const stopTimer = () => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    if (scoreboard.timerInterval !== null) {
      clearInterval(scoreboard.timerInterval);
      scoreboard.timerInterval = null;
    }
  };

  // 获取当前记分牌的辅助函数
  const getCurrentScoreboard = () => {
    return scoreboards.value[0] || null;
  };

  // 以下是对当前记分牌的操作
  const setHomeTeamName = (name: string) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.homeTeam.name = name;
    }
  };

  const setGuestTeamName = (name: string) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.guestTeam.name = name;
    }
  };

  const increaseScore = (team: 'home' | 'guest') => {
    const current = getCurrentScoreboard();
    if (current) {
      if (team === 'home') {
        current.homeTeam.score += 1;
      } else {
        current.guestTeam.score += 1;
      }
    }
  };

  const decreaseScore = (team: 'home' | 'guest') => {
    const current = getCurrentScoreboard();
    if (current) {
      if (team === 'home' && current.homeTeam.score > 0) {
        current.homeTeam.score -= 1;
      } else if (team === 'guest' && current.guestTeam.score > 0) {
        current.guestTeam.score -= 1;
      }
    }
  };

  const setHomeTeamLogo = (logo: File | null, url: string | null) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.homeTeam.logo = logo;
      current.homeTeam.logoUrl = url;
    }
  };

  const setGuestTeamLogo = (logo: File | null, url: string | null) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.guestTeam.logo = logo;
      current.guestTeam.logoUrl = url;
    }
  };

  const toggleClock = () => {
    const current = getCurrentScoreboard();
    if (current) {
      current.isRunning = !current.isRunning;

      // 根据运行状态启动或停止定时器
      if (current.isRunning) {
        startTimer();
      } else {
        stopTimer();
      }
    }
  };

  const setTime = (minutes: number, seconds: number) => {
    const current = getCurrentScoreboard();
    if (current) {
      const totalSeconds = minutes * 60 + seconds;
      current.currentTime = totalSeconds;
    }
  };

  const adjustTime = (seconds: number) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.currentTime = Math.max(0, current.currentTime + seconds);
    }
  };

  const toggleCountdownDirection = () => {
    const current = getCurrentScoreboard();
    if (current) {
      current.isCountdown = !current.isCountdown;
    }
  };

  const setDisplayScale = (scale: number) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.displayScale = scale;
    }
  };

  const setShowScoreboard = (show: boolean) => {
    const current = getCurrentScoreboard();
    if (current) {
      current.visible = show;
    }
  };

  const resetScores = () => {
    const current = getCurrentScoreboard();
    if (current) {
      current.homeTeam.score = 0;
      current.guestTeam.score = 0;
      current.currentTime = 0;
      current.isRunning = false;

      // 停止定时器
      if (current.timerInterval !== null) {
        clearInterval(current.timerInterval);
        current.timerInterval = null;
      }
    }
  };

  const resetAll = () => {
    const current = getCurrentScoreboard();
    if (current) {
      resetScores();
      current.homeTeam.name = '主队';
      current.guestTeam.name = '客队';
      current.homeTeam.logo = null;
      current.homeTeam.logoUrl = null;
      current.guestTeam.logo = null;
      current.guestTeam.logoUrl = null;
      current.isCountdown = true;
      current.displayScale = 1;
      current.titleText1 = '';
      current.titleText2 = '';
      current.titleText3 = '';
      current.visible = true;
    }
  };

  // 新增功能：更新记分牌的属性
  const updateScoreboardProperty = <T extends keyof ScoreboardData>(
    property: T,
    value: ScoreboardData[T],
  ) => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    // 检查属性是否为合法的ScoreboardData键，并且不是函数类型的属性
    if (property in scoreboard && typeof value !== 'function') {
      scoreboard[property] = value;
    }
  };

  // 新增功能：更新队伍分数
  const updateTeamScore = (isHome: boolean, delta: number) => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    if (isHome) {
      scoreboard.homeTeam.score = Math.max(0, scoreboard.homeTeam.score + delta);
    } else {
      scoreboard.guestTeam.score = Math.max(0, scoreboard.guestTeam.score + delta);
    }
  };

  // 新增功能：重置队伍分数
  const resetTeamScore = (isHome: boolean) => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    if (isHome) {
      scoreboard.homeTeam.score = 0;
    } else {
      scoreboard.guestTeam.score = 0;
    }
  };

  // 新增功能：调整时钟
  const adjustClock = (delta: number) => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    scoreboard.currentTime = Math.max(0, scoreboard.currentTime + delta);
  };

  // 新增功能：重置时钟
  const resetClock = () => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    scoreboard.currentTime = 0;
    scoreboard.isRunning = false;
    stopTimer();
  };

  // 新增功能：设置时钟时间
  const setClockTime = (time: number) => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    scoreboard.currentTime = time;
  };

  // 新增功能：设置队伍Logo
  const setTeamLogo = (isHome: boolean, logoUrl: string) => {
    const scoreboard = scoreboards.value[0];
    if (!scoreboard) return;

    if (isHome) {
      scoreboard.homeTeam.logoUrl = logoUrl;
    } else {
      scoreboard.guestTeam.logoUrl = logoUrl;
    }
  };

  // 保存状态到 localStorage
  const saveToLocalStorage = () => {
    try {
      const stateToSave = {
        scoreboards: scoreboards.value.map((sb) => {
          // 不保存定时器ID，因为它不会在刷新后有效
          // eslint-disable-next-line @typescript-eslint/no-unused-vars
          const { timerInterval, ...rest } = sb;
          return rest;
        }),
        activeScoreboardId: activeScoreboardId.value,
      };
      localStorage.setItem('scoreboardState', JSON.stringify(stateToSave));
    } catch (error) {
      console.error('保存记分牌状态失败:', error);
    }
  };

  // 从 localStorage 加载状态
  const loadFromLocalStorage = () => {
    try {
      const savedState = localStorage.getItem('scoreboardState');
      if (savedState) {
        const parsedState: StoredState = JSON.parse(savedState);

        // 重新构建记分牌数据，恢复定时器状态
        const loadedScoreboards: ScoreboardData[] = parsedState.scoreboards.map(
          (sb: StoredScoreboardData) => ({
            ...sb,
            timerInterval: null, // 重新开始时没有活动的定时器
            homeTeam: {
              ...sb.homeTeam,
              logo: null, // File 对象无法序列化，设为 null
            },
            guestTeam: {
              ...sb.guestTeam,
              logo: null, // File 对象无法序列化，设为 null
            },
          }),
        );

        scoreboards.value = loadedScoreboards;
        activeScoreboardId.value = parsedState.activeScoreboardId;

        // 如果记分牌之前正在运行，需要重启定时器
        const current = getCurrentScoreboard();
        if (current && current.isRunning) {
          startTimer();
        }
      } else {
        // 如果没有保存的数据，创建新的记分牌
        createScoreboard();
      }
    } catch (error) {
      console.error('加载记分牌状态失败:', error);
      // 如果加载失败，创建一个新的记分牌
      createScoreboard();
    }
  };

  // 初始化记分牌 - 先尝试从本地存储加载，如果没有再创建新的
  loadFromLocalStorage();

  // 每隔一段时间自动保存状态
  setInterval(() => {
    saveToLocalStorage();
  }, 1000); // 每秒保存一次

  return {
    // State
    scoreboards,
    activeScoreboardId,

    // Computed
    formattedTime,
    clockStateText,
    currentScoreboard: getCurrentScoreboard, // 保持与现有代码的兼容性

    // Actions
    createScoreboard,
    setActiveScoreboard,
    startTimer,
    stopTimer,
    setHomeTeamName,
    setGuestTeamName,
    increaseScore,
    decreaseScore,
    setHomeTeamLogo,
    setGuestTeamLogo,
    toggleClock,
    setTime,
    adjustTime,
    toggleCountdownDirection,
    setDisplayScale,
    setShowScoreboard,
    resetScores,
    resetAll,
    updateScoreboardProperty,
    updateTeamScore,
    resetTeamScore,
    adjustClock,
    resetClock,
    setClockTime,
    setTeamLogo,
    // 新增的持久化功能
    saveToLocalStorage,
    loadFromLocalStorage,
  };
});
