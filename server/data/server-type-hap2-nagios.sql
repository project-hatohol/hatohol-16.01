INSERT INTO server_types (type, name, parameters, plugin_path, plugin_sql_version, plugin_enabled, uuid)
  VALUES (7, 'Nagios (HAPI2) [experimental]', '[{"id": "nickname", "label": "Nickname"}, {"hint": "IP_ADDRESS:PORT/DB_NAME", "id": "baseURL", "label": "URL"}, {"id": "userName", "label": "User name"}, {"inputStyle": "password", "id": "password", "label": "Password"}, {"default": "30", "id": "pollingInterval", "label": "Polling interval (sec)"}, {"default": "10", "id": "retryInterval", "label": "Retry interval (sec)"}, {"inputStyle": "checkBox", "id": "passiveMode", "label": "Passive mode"}, {"hint": "amqp://user:password@localhost/vhost", "id": "brokerUrl", "label": "Broker URL"}, {"hint": "(empty: Default)", "allowEmpty": true, "id": "staticQueueAddress", "label": "Static queue address"}, {"allowEmpty": true, "id": "tlsCertificatePath", "label": "TLS client certificate path"}, {"allowEmpty": true, "id": "tlsKeyPath", "label": "TLS client key path"}, {"allowEmpty": true, "id": "tlsCACertificatePath", "label": "TLS CA certificate path"}, {"inputStyle": "checkBox", "allowEmpty": true, "id": "tlsEnableVerify", "label": "TLS: Enable verify"}]', 'start-stop-hap2-nagios-ndoutils.sh', 1, 1, '902d955c-d1f7-11e4-80f9-d43d7e3146fb')
  ON DUPLICATE KEY UPDATE name='Nagios (HAPI2) [experimental]', parameters='[{"id": "nickname", "label": "Nickname"}, {"hint": "IP_ADDRESS:PORT/DB_NAME", "id": "baseURL", "label": "URL"}, {"id": "userName", "label": "User name"}, {"inputStyle": "password", "id": "password", "label": "Password"}, {"default": "30", "id": "pollingInterval", "label": "Polling interval (sec)"}, {"default": "10", "id": "retryInterval", "label": "Retry interval (sec)"}, {"inputStyle": "checkBox", "id": "passiveMode", "label": "Passive mode"}, {"hint": "amqp://user:password@localhost[:port]/vhost", "id": "brokerUrl", "label": "Broker URL"}, {"hint": "(empty: Default)", "allowEmpty": true, "id": "staticQueueAddress", "label": "Static queue address"}, {"allowEmpty": true, "id": "tlsCertificatePath", "label": "TLS client certificate path"}, {"allowEmpty": true, "id": "tlsKeyPath", "label": "TLS client key path"}, {"allowEmpty": true, "id": "tlsCACertificatePath", "label": "TLS CA certificate path"}, {"inputStyle": "checkBox", "allowEmpty": true, "id": "tlsEnableVerify", "label": "TLS: Enable verify"}]', plugin_path='start-stop-hap2-nagios-ndoutils.sh', plugin_sql_version='1', plugin_enabled='1';
