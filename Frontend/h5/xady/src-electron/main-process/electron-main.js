import { app, BrowserWindow, ipcMain } from "electron";
import fileStream from "fs";

/**
 * Set `__statics` path to static files in production;
 * The reason we are setting it here is that the path needs to be evaluated at runtime
 */
if (process.env.PROD) {
  global.__statics = require("path")
    .join(__dirname, "statics")
    .replace(/\\/g, "\\\\");
}

const gotTheLock = app.requestSingleInstanceLock();

if (!gotTheLock) {
  app.quit();
} else {
  app.on("second-instance", () => {
    if (mainWindow) {
      mainWindow.show();
    }
  });
}

let mainWindow;

function createWindow() {
  const packageJson = GetJson("package.json");

  /**
   * Initial window options
   */
  mainWindow = new BrowserWindow({
    title: packageJson.productName,
    minWidth: 1366,
    minHeight: 768,
    width: 1366,
    height: 768,
    frame: false,
    show: false,
    center: true,
    icon: __statics + "/app-logo-128x128.png",
    transparent: true,
    webPreferences: {
      nodeIntegration: true,
      nodeIntegrationInWorker: true,
      webviewTag: true
    }
  });

  mainWindow.loadURL(process.env.APP_URL + "#/index");

  mainWindow.on("closed", () => {
    mainWindow = null;
  });

  mainWindow.once("ready-to-show", () => {
    mainWindow.show();
    mainWindow.center();
    mainWindow.focus();
  });
}

app.on("ready", createWindow);

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});

app.on("activate", () => {
  if (mainWindow === null) {
    createWindow();
  }
});

ipcMain.on("newWindow", (event, arg) => {
  const packageJson = GetJson("package.json");

  let window = new BrowserWindow({
    title: packageJson.productName,
    minWidth: 1280,
    minHeight: 720,
    width: 1280,
    height: 720,
    frame: false,
    show: false,
    center: true,
    icon: __statics + "/app-logo-128x128.png",
    transparent: true,
    webPreferences: {
      nodeIntegration: true,
      nodeIntegrationInWorker: true,
      webviewTag: true
    }
  });

  window.loadURL(process.env.APP_URL + arg);

  window.on("closed", () => {
    window = null;
  });

  window.once("ready-to-show", () => {
    window.show();
    window.center();
    window.focus();
  });
});

function GetJson(file) {
  file = app.getAppPath() + "\\" + file;
  return JSON.parse(fileStream.readFileSync(file).toString());
}
