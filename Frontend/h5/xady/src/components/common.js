import fileStream from "fs";
import electron from "electron";

export default {
  SimlateURL(url, callback) {
    let view = new electron.remote.BrowserView();

    view.webContents.loadURL(url, {
      httpReferrer: url
    });

    view.webContents.on("did-fail-load", (code, error) => {
      callback("fail", error);
      view.destroy();
    });

    view.webContents.on("did-finish-load", () => {
      view.webContents
        .executeJavaScript(`document.body.innerHTML;`)
        .then(result => {
          callback("success", result);
          view.destroy();
        });
    });
  },

  ipc(event, arg1, arg2, arg3) {
    electron.ipcRenderer.send(event, arg1, arg2, arg3);
  },

  GetJson(file) {
    file = electron.remote.app.getAppPath() + "\\" + file;
    return JSON.parse(fileStream.readFileSync(file).toString());
  },

  SaveJson(data, file) {
    file = electron.remote.app.getAppPath() + "\\" + file;
    fileStream.writeFile(file, data);
  }
};
