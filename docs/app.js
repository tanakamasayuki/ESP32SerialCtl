document.addEventListener('DOMContentLoaded', () => {
  const tabButtons = document.querySelectorAll('.tab-button');
  const tabPanels = document.querySelectorAll('.tab-content');

  const storageMocks = {
    list: `OK storage list
|   sd (SD card) [not mounted]
|   spiffs (SPIFFS) [mounted]
|   littlefs (LittleFS) [not mounted]
|   fatfs (FAT FS) [not mounted]`,
    statusNone: `ERR 412 No storage selected`,
    statusById: {
      spiffs: `OK storage status
| storage: spiffs
| mounted: yes
| total: 1318001 bytes
| used: 1506 bytes
| free: 1316495 bytes`
    }
  };

  const fsMocks = {
    spiffs: {
      note: 'SPIFFS は階層ディレクトリを持たないため、すべてのファイルはルート (/) 直下で管理します。',
      listRaw: `OK fs ls
| storage: spiffs path: /
|          9 aaa.txt
|         13 base64.txt
|         12 test.txt
|          8 aaa2.txt
|          1 base.txt`,
      tree: [
        {
          name: 'aaa.txt',
          path: '/aaa.txt',
          type: 'file',
          size: '9 bytes',
          cat: `OK fs cat
| storage: spiffs path: /aaa.txt offset: 0
| length: until EOF
| テスト`,
          stat: `OK fs stat
| storage: spiffs path: /aaa.txt
| type: file
| size: 9 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /aaa.txt
| algo: sha256
| size: 9 bytes
| digest: --`
        },
        {
          name: 'base64.txt',
          path: '/base64.txt',
          type: 'file',
          size: '13 bytes',
          cat: `OK fs cat
| storage: spiffs path: /base64.txt offset: 0
| length: until EOF
| (Base64 サンプルデータ)`,
          stat: `OK fs stat
| storage: spiffs path: /base64.txt
| type: file
| size: 13 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /base64.txt
| algo: sha256
| size: 13 bytes
| digest: --`
        },
        {
          name: 'test.txt',
          path: '/test.txt',
          type: 'file',
          size: '12 bytes',
          cat: `OK fs cat
| storage: spiffs path: /test.txt offset: 0
| length: until EOF
| test string`,
          stat: `OK fs stat
| storage: spiffs path: /test.txt
| type: file
| size: 12 bytes
| modified: 2695`,
          b64read: `OK fs b64read
| storage: spiffs path: /test.txt
| chunk: 48
| data[0]: dGVzdCBzdHJpbmcK
| chunks: 1`,
          hash: `OK fs hash
| storage: spiffs path: /test.txt
| algo: sha256
| size: 12 bytes
| digest: 37d2046a395cbfcb2712ff5c96a727b1966876080047c56717009dbbc235f566`
        },
        {
          name: 'aaa2.txt',
          path: '/aaa2.txt',
          type: 'file',
          size: '8 bytes',
          cat: `OK fs cat
| storage: spiffs path: /aaa2.txt offset: 0
| length: until EOF
| test str`,
          stat: `OK fs stat
| storage: spiffs path: /aaa2.txt
| type: file
| size: 8 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /aaa2.txt
| algo: sha256
| size: 8 bytes
| digest: --`
        },
        {
          name: 'base.txt',
          path: '/base.txt',
          type: 'file',
          size: '1 bytes',
          cat: `OK fs cat
| storage: spiffs path: /base.txt offset: 0
| length: until EOF
| A`,
          stat: `OK fs stat
| storage: spiffs path: /base.txt
| type: file
| size: 1 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /base.txt
| algo: sha256
| size: 1 bytes
| digest: --`
        }
      ]
    },
    littlefs: {
      note: 'LittleFS はディレクトリ階層を利用できます。必要に応じてルート以外のパスも取得します。',
      listRaw: `OK fs ls
| storage: littlefs path: /
|         12 test.txt
| <DIR> test

OK fs ls
| storage: littlefs path: /test
|          3 aaa.txt`,
      tree: [
        {
          name: 'test.txt',
          path: '/test.txt',
          type: 'file',
          size: '12 bytes',
          cat: `OK fs cat
| storage: littlefs path: /test.txt offset: 0
| length: until EOF
| test string`,
          stat: `OK fs stat
| storage: littlefs path: /test.txt
| type: file
| size: 12 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: littlefs path: /test.txt
| algo: sha256
| size: 12 bytes
| digest: --`
        },
        {
          name: 'test',
          path: '/test',
          type: 'dir',
          size: '--',
          lsRaw: `OK fs ls
| storage: littlefs path: /test
|          3 aaa.txt`,
          stat: `OK fs stat
| storage: littlefs path: /test
| type: dir
| size: 3 bytes
| entries: 1`,
          children: [
            {
              name: 'aaa.txt',
              path: '/test/aaa.txt',
              type: 'file',
              size: '3 bytes',
              cat: `OK fs cat
| storage: littlefs path: /test/aaa.txt offset: 0
| length: until EOF
| aaa`,
              stat: `OK fs stat
| storage: littlefs path: /test/aaa.txt
| type: file
| size: 3 bytes
| modified: 2695`,
              b64read: null,
              hash: `OK fs hash
| storage: littlefs path: /test/aaa.txt
| algo: sha256
| size: 3 bytes
| digest: --`
            }
          ]
        }
      ]
    }
  };

  const storageElements = {
    select: document.querySelector('#storage-select'),
    listTableBody: document.querySelector('[data-storage-list-table] tbody'),
    listRaw: document.querySelector('[data-storage-list-raw]'),
    listTimestamp: document.querySelector('[data-storage-list-timestamp]'),
    statusTable: document.querySelector('[data-storage-status-table]'),
    statusTableBody: document.querySelector('[data-storage-status-table] tbody'),
    statusRaw: document.querySelector('[data-storage-status-raw]'),
    statusTimestamp: document.querySelector('[data-storage-status-timestamp]')
  };

  const fsElements = {
    tree: document.querySelector('[data-fs-tree]'),
    empty: document.querySelector('[data-fs-empty]'),
    listRaw: document.querySelector('[data-fs-list-raw]'),
    listTimestamp: document.querySelector('[data-fs-list-timestamp]'),
    infoMessage: document.querySelector('[data-fs-info-message]'),
    selectedPath: document.querySelector('[data-fs-selected-path]'),
    detailTable: document.querySelector('[data-fs-detail-table]'),
    detailTableBody: document.querySelector('[data-fs-detail-table] tbody'),
    dirSection: document.querySelector('[data-fs-dir-section]'),
    dirRaw: document.querySelector('[data-fs-dir-raw]'),
    catSection: document.querySelector('[data-fs-cat-section]'),
    catRaw: document.querySelector('[data-fs-cat-raw]'),
    b64Section: document.querySelector('[data-fs-b64-section]'),
    b64Raw: document.querySelector('[data-fs-b64-raw]'),
    hashSection: document.querySelector('[data-fs-hash-section]'),
    hashRaw: document.querySelector('[data-fs-hash-raw]')
  };

  let storageInitialized = false;
  let currentStorageId = null;
  let currentTab = 'system';
  let currentFsSelection = null;
  let fsPathMap = new Map();
  let lastStorageStatusRaw = storageMocks.statusNone;

  if (fsElements.tree && !fsElements.tree.dataset.bound) {
    fsElements.tree.addEventListener('click', (event) => {
      const node = event.target.closest('.fs-node');
      if (!node) {
        return;
      }
      const path = node.dataset.fsPath;
      if (path) {
        selectFsPath(path);
      }
    });
    fsElements.tree.dataset.bound = 'true';
  }

  const getStorageListRaw = (selectedId) =>
    storageMocks.list
      .split('\n')
      .map((line) => {
        const match = line.match(/\|\s+([^\s]+)\s+\(([^)]+)\)\s+\[(.+)\]/);
        if (!match) {
          return line;
        }
        const id = match[1];
        const state = selectedId && id === selectedId ? 'mounted' : 'not mounted';
        return line.replace(/\[(.+)\]/, `[${state}]`);
      })
      .join('\n');

  const escapeSelector = (value) => {
    if (window.CSS && typeof window.CSS.escape === 'function') {
      return window.CSS.escape(value);
    }
    return value.replace(/([\\.\\#:\\[\\]])/g, '\\\\$1');
  };

  const activateTab = (targetId) => {
    currentTab = targetId;
    tabButtons.forEach((button) => {
      const isActive = button.dataset.tab === targetId;
      button.classList.toggle('is-active', isActive);
      button.setAttribute('aria-selected', isActive ? 'true' : 'false');
    });

    tabPanels.forEach((panel) => {
      const isActive = panel.id === `tab-${targetId}`;
      panel.classList.toggle('is-active', isActive);
    });

    if (targetId === 'storage') {
      runStorageAutoFetch();
    }
    if (targetId === 'filesystem') {
      runFsAutoFetch();
    }
  };

  tabButtons.forEach((button) => {
    button.addEventListener('click', () => {
      activateTab(button.dataset.tab);
    });
  });

  activateTab('system');

  const systemMockRaw = {
    'sys-info': `OK sys info
| Board: ESP32_DEV
| Variant: esp32
| FQBN: esp32:esp32:esp32:DebugLevel=verbose
| Chip: ESP32-D0WD-V3 rev 301
| Cores: 2
| CPU Frequency: 240 MHz
| Chip Features: WiFi/BT/BLE
| Flash: 4194304 bytes @ 80000000 Hz
| MAC: 88:AE:6F:34:E3:EC
| IDF: v5.5.1-255-g07e9bf4970
| Arduino Core: 3_3_2
| Build: Oct 21 2025 15:43:05`,
    'sys-uptime': `OK sys uptime
| Uptime: 0:23:28 (1408444 ms)`,
    'sys-time': `OK sys time
| localtime: 1970-01-01T00:23:32+00:00`,
    'sys-mem': `OK sys mem
| Heap Total: 306664 bytes
| Heap Free: 256336 bytes
| Heap Min Free: 245608 bytes
| Heap Largest Block: 110580 bytes
| Internal Total: 361160 bytes
| Internal Free: 309980 bytes
| Internal Min Free: 299208 bytes
| Internal Largest Block: 110580 bytes
| RTOS Heap Free: 256336 bytes
| RTOS Heap Min Free: 245608 bytes
| Task Stack High Water: 3952 words`
  };

  const parsePipeTable = (raw) =>
    raw
      .split('\n')
      .filter((line) => line.trim().startsWith('|'))
      .map((line) => line.replace(/^\|\s*/, ''))
      .map((line) => {
        const [key, ...rest] = line.split(':');
        return {
          key: key.trim(),
          value: rest.join(':').trim()
        };
      });

  const formatTimeStamp = () =>
    new Intl.DateTimeFormat('ja-JP', {
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit'
    }).format(new Date());

  const updateResultSection = (panel, commandId) => {
    const raw = systemMockRaw[commandId];
    if (!raw) {
      return;
    }

    const timestampEl = panel.querySelector('[data-result-timestamp]');
    const rawEl = panel.querySelector('[data-result-raw]');
    const tableEl = panel.querySelector('[data-result-table]');
    const tableBody = tableEl ? tableEl.querySelector('tbody') : null;

    if (timestampEl) {
      timestampEl.textContent = formatTimeStamp();
    }
    if (rawEl) {
      rawEl.textContent = raw;
    }
    if (tableEl && tableBody) {
      const rows = parsePipeTable(raw);
      tableBody.innerHTML = '';
      rows.forEach(({ key, value }) => {
        const row = document.createElement('tr');
        const keyCell = document.createElement('th');
        keyCell.textContent = key;
        const valueCell = document.createElement('td');
        valueCell.textContent = value;
        row.append(keyCell, valueCell);
        tableBody.append(row);
      });
      tableEl.hidden = rows.length === 0;
    }
  };

  document.querySelectorAll('.command-layout').forEach((layout) => {
    const commandTabs = layout.querySelectorAll('.command-tab');
    const commandPanels = layout.querySelectorAll('.command-panel');

    if (!commandTabs.length) {
      return;
    }

    const activateCommand = (commandId) => {
      commandTabs.forEach((tab) => {
        const isActive = tab.dataset.command === commandId;
        tab.classList.toggle('is-active', isActive);
        tab.setAttribute('aria-selected', isActive ? 'true' : 'false');
      });

      let activePanel = null;
      commandPanels.forEach((panel) => {
        const isActive = panel.dataset.command === commandId;
        panel.classList.toggle('is-active', isActive);
        if (isActive) {
          activePanel = panel;
        }
      });

      if (activePanel && activePanel.dataset.autoFetch === 'true') {
        updateResultSection(activePanel, commandId);
      }
    };

    commandTabs.forEach((tab) => {
      tab.addEventListener('click', () => {
        activateCommand(tab.dataset.command);
      });
    });

    activateCommand(commandTabs[0].dataset.command);
  });

  const parseStorageList = (raw) =>
    raw
      .split('\n')
      .filter((line) => line.trim().startsWith('|'))
      .map((line) => line.replace(/^\|\s*/, '').trim())
      .map((line) => {
        const match = line.match(/^([^\s]+)\s*\(([^)]+)\)\s*\[(.+)\]$/);
        if (!match) {
          return null;
        }
        const [, id, label, status] = match;
        const mounted = status.trim() === 'mounted';
        return {
          id,
          label,
          status: status.trim(),
          mounted
        };
      })
      .filter(Boolean);

  const renderStorageList = (raw, selectedId = '') => {
    if (storageElements.listTimestamp) {
      storageElements.listTimestamp.textContent = formatTimeStamp();
    }
    if (storageElements.listRaw) {
      storageElements.listRaw.textContent = raw;
    }
    const entries = parseStorageList(raw);
    if (storageElements.listTableBody) {
      storageElements.listTableBody.innerHTML = '';
      entries.forEach((entry) => {
        const row = document.createElement('tr');
        row.classList.toggle('is-mounted', entry.mounted);
        const nameCell = document.createElement('td');
        nameCell.textContent = entry.id;
        const typeCell = document.createElement('td');
        typeCell.textContent = entry.label;
        const statusCell = document.createElement('td');
        statusCell.textContent = entry.status;
        row.append(nameCell, typeCell, statusCell);
        storageElements.listTableBody.append(row);
      });
    }
    updateStorageSelect(entries, selectedId);
    return entries;
  };

  const renderStorageStatus = (raw) => {
    if (storageElements.statusTimestamp) {
      storageElements.statusTimestamp.textContent = formatTimeStamp();
    }
    if (storageElements.statusRaw) {
      storageElements.statusRaw.textContent = raw;
    }
    const rows = parsePipeTable(raw);
    if (storageElements.statusTable && storageElements.statusTableBody) {
      storageElements.statusTableBody.innerHTML = '';
      rows.forEach(({ key, value }) => {
        const row = document.createElement('tr');
        const keyCell = document.createElement('th');
        keyCell.textContent = key;
        const valueCell = document.createElement('td');
        valueCell.textContent = value;
        row.append(keyCell, valueCell);
        storageElements.statusTableBody.append(row);
      });
      storageElements.statusTable.hidden = rows.length === 0;
    }
  };

  const updateStorageSelect = (entries, selectedId = '') => {
    if (!storageElements.select) {
      return;
    }
    storageElements.select.innerHTML = '';
    const mountedEntries = entries.filter((entry) => entry.mounted);
    if (!mountedEntries.length) {
      const option = document.createElement('option');
      option.value = '';
      option.textContent = 'マウント済みのストレージがありません';
      storageElements.select.append(option);
      storageElements.select.disabled = true;
      return;
    }
    const placeholder = document.createElement('option');
    placeholder.value = '';
    placeholder.textContent = 'ストレージを選択...';
    storageElements.select.append(placeholder);
    mountedEntries.forEach((entry) => {
      const option = document.createElement('option');
      option.value = entry.id;
      option.textContent = `${entry.id} (${entry.label})`;
      storageElements.select.append(option);
    });
    storageElements.select.disabled = false;
    storageElements.select.value = selectedId;
  };

  const runStorageAutoFetch = () => {
    renderStorageList(getStorageListRaw(currentStorageId), currentStorageId || '');
    renderStorageStatus(lastStorageStatusRaw);

    if (!storageInitialized && storageElements.select) {
      storageElements.select.addEventListener('change', (event) => {
        if (!event.target.value) {
          return;
        }
        handleStorageSelection(event.target.value);
      });
      storageInitialized = true;
    }
  };

  const handleStorageSelection = (storageId) => {
    if (!storageId) {
      return;
    }
    currentStorageId = storageId;
    currentFsSelection = null;
    renderStorageList(getStorageListRaw(storageId), storageId);
    const useRaw = `OK storage use\n| current: ${storageId}`;
    const statusRaw = storageMocks.statusById[storageId] || storageMocks.statusNone;
    const combinedStatusRaw = `${useRaw}\n${statusRaw}`;
    lastStorageStatusRaw = combinedStatusRaw;
    renderStorageStatus(combinedStatusRaw);
    runFsAutoFetch();
  };

  const resetFsDetails = () => {
    if (fsElements.selectedPath) {
      fsElements.selectedPath.textContent = '--';
    }
    if (fsElements.detailTable) {
      fsElements.detailTable.hidden = true;
    }
    if (fsElements.detailTableBody) {
      fsElements.detailTableBody.innerHTML = '';
    }
    if (fsElements.dirSection) {
      fsElements.dirSection.hidden = true;
    }
    if (fsElements.dirRaw) {
      fsElements.dirRaw.textContent = 'ディレクトリの内容がここに表示されます';
    }
    if (fsElements.catSection) {
      fsElements.catSection.hidden = true;
    }
    if (fsElements.catRaw) {
      fsElements.catRaw.textContent = 'ファイル内容がここに表示されます';
    }
    if (fsElements.b64Section) {
      fsElements.b64Section.hidden = true;
    }
    if (fsElements.b64Raw) {
      fsElements.b64Raw.textContent = 'Base64 形式の読み出し結果がここに表示されます';
    }
    if (fsElements.hashSection) {
      fsElements.hashSection.hidden = true;
    }
    if (fsElements.hashRaw) {
      fsElements.hashRaw.textContent = 'ハッシュ結果がここに表示されます';
    }
    currentFsSelection = null;
  };

  const clearFsView = (message) => {
    fsPathMap = new Map();
    if (fsElements.tree) {
      fsElements.tree.innerHTML = '';
      const p = document.createElement('p');
      p.className = 'fs-empty';
      p.textContent = message;
      fsElements.tree.append(p);
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = '結果がここに表示されます';
    }
    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = '--:--';
    }
    if (fsElements.infoMessage) {
      fsElements.infoMessage.textContent =
        'ファイルまたはディレクトリを選択すると詳細が表示されます。';
    }
    resetFsDetails();
  };

  const buildFsTree = (items = []) => {
    const ul = document.createElement('ul');
    ul.className = 'fs-tree-list';
    items.forEach((item) => {
      fsPathMap.set(item.path, item);
      const li = document.createElement('li');
      li.classList.add('fs-tree-item', item.type === 'dir' ? 'is-dir' : 'is-file');
      const button = document.createElement('button');
      button.type = 'button';
      button.className = 'fs-node';
      button.dataset.fsPath = item.path;
      button.dataset.fsType = item.type;
      button.textContent = item.type === 'dir' ? `${item.name}/` : item.name;
      li.append(button);
      if (item.type === 'dir' && item.children && item.children.length) {
        li.append(buildFsTree(item.children));
      }
      ul.append(li);
    });
    return ul;
  };

  const findFirstSelectableNode = (items = []) => {
    for (const item of items) {
      if (item.type === 'file') {
        return item;
      }
      if (item.type === 'dir' && item.children && item.children.length) {
        const found = findFirstSelectableNode(item.children);
        if (found) {
          return found;
        }
      }
    }
    return items[0] || null;
  };

  const selectFsPath = (path) => {
    if (!fsElements.tree) {
      return;
    }
    const node = fsPathMap.get(path);
    if (!node) {
      return;
    }
    const previous = fsElements.tree.querySelector('.fs-node.is-active');
    if (previous) {
      previous.classList.remove('is-active');
    }
    const currentButton = fsElements.tree.querySelector(`[data-fs-path=\"${CSS.escape(path)}\"]`);
    if (currentButton) {
      currentButton.classList.add('is-active');
    }
    currentFsSelection = path;
    updateFsDetail(node);
  };

  const updateFsDetail = (node) => {
    if (!node) {
      resetFsDetails();
      return;
    }

    if (fsElements.selectedPath) {
      fsElements.selectedPath.textContent = node.path;
    }

    if (fsElements.detailTableBody) {
      const rows = [];
      rows.push({
        key: 'タイプ',
        value: node.type === 'dir' ? 'ディレクトリ' : 'ファイル'
      });
      rows.push({
        key: 'サイズ',
        value: node.size || '--'
      });
      rows.push({
        key: 'ストレージ',
        value: currentStorageId || '--'
      });
      if (node.type === 'dir' && node.children) {
        rows.push({
          key: '子要素',
          value: `${node.children.length} 件`
        });
      }
      if (fsElements.detailTable) {
        fsElements.detailTable.hidden = false;
      }
      fsElements.detailTableBody.innerHTML = '';
      rows.forEach(({ key, value }) => {
        const tr = document.createElement('tr');
        const th = document.createElement('th');
        th.textContent = key;
        const td = document.createElement('td');
        td.textContent = value;
        tr.append(th, td);
        fsElements.detailTableBody.append(tr);
      });
    }

    if (fsElements.dirSection) {
      fsElements.dirSection.hidden = !(node.type === 'dir' && node.lsRaw);
    }
    if (fsElements.dirRaw && node.lsRaw) {
      fsElements.dirRaw.textContent = node.lsRaw;
    }

    if (fsElements.catSection) {
      fsElements.catSection.hidden = !(node.type === 'file' && node.cat);
    }
    if (fsElements.catRaw && node.cat) {
      fsElements.catRaw.textContent = node.cat;
    }

    if (fsElements.b64Section) {
      fsElements.b64Section.hidden = !(node.type === 'file' && node.b64read);
    }
    if (fsElements.b64Raw && node.b64read) {
      fsElements.b64Raw.textContent = node.b64read;
    }

    if (fsElements.hashSection) {
      fsElements.hashSection.hidden = !(node.type === 'file' && node.hash);
    }
    if (fsElements.hashRaw && node.hash) {
      fsElements.hashRaw.textContent = node.hash;
    }
  };

  const runFsAutoFetch = () => {
    if (!fsElements.tree) {
      return;
    }
    if (!currentStorageId) {
      clearFsView('ストレージタブで対象を選択してください。');
      return;
    }
    const fsData = fsMocks[currentStorageId];
    if (!fsData) {
      clearFsView('このストレージのモックデータはまだ用意されていません。');
      return;
    }

    if (fsElements.infoMessage) {
      fsElements.infoMessage.textContent = fsData.note;
    }
    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = formatTimeStamp();
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = fsData.listRaw;
    }

    fsPathMap = new Map();
    if (fsElements.tree) {
      fsElements.tree.innerHTML = '';
      fsElements.tree.append(buildFsTree(fsData.tree));
    }
    currentFsSelection = null;

    const defaultNode = findFirstSelectableNode(fsData.tree);
    if (defaultNode) {
      selectFsPath(defaultNode.path);
    } else {
      resetFsDetails();
    }
  };

  const connectButton = document.querySelector('#connect-button');
  const disconnectButton = document.querySelector('#disconnect-button');
  const statusLabel = document.querySelector('#connection-status-label');

  const placeholders = [
    connectButton,
    disconnectButton,
    ...document.querySelectorAll(
      'button[disabled], select[disabled], input[disabled], textarea[disabled]'
    )
  ];

  placeholders.forEach((element) => {
    if (!element) {
      return;
    }
    element.setAttribute('title', 'モック画面のため現在は操作できません');
  });

  if (statusLabel) {
    statusLabel.textContent = 'モック画面 - 動作実装前';
  }
});
