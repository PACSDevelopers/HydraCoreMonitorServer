<?php

namespace HCMS;

class Session extends \HC\Core implements \SessionHandlerInterface
{
    protected $connection = null;
    static protected $lifetime = 3600;

    function open($savePath, $sessionName)
    {
        $this->connection = new \HC\DB();
        if($this->connection) {
            return true;
        }
        return false;
    }

    function close()
    {
        return true;
    }

    function read($id)
    {
        $result = $this->connection->query('SELECT `data` FROM `sessions` WHERE `hash` = ?;', [$id]);
        if($result) {
            return $result[0]['data'];
        } else {
          $result = $this->generateNewSession();
        }

        return '';
    }

    function write($sessionID, $data)
    {
        $result = $this->connection->query('SELECT `id`, `data` FROM `sessions` WHERE `hash` = ? AND `lifeTime` > ?;', [$sessionID, time()]);
        if(!$result) {
            return $this->generateNewSession($data);
        } else {
          // Old session, check if the data has actually changed
          if($data !== $result[0]['data']) {
            // It has, update it
            $result = $this->connection->query('UPDATE `sessions` SET `data` = ? WHERE `id` = ?;', [$data, $result[0]['id']]);
            if($result) {
                return true;
            }
          } else {
            // It hasn't, return true
            return true;
          }
        }

        // A query failed, return false
        return false;
    }

    function destroy($id)
    {
        $this->gc(1);
        $result = $this->connection->query('DELETE FROM `sessions` WHERE `hash` = ?;', [$id]);
        if($result){
            return true;
        }
        return false;
    }

    function gc($maxlifetime)
    {
        $result = $this->connection->query('DELETE FROM `sessions` WHERE `lifeTime` < ?;', [time()]);
        if($result) {
            return true;
        }
        return false;
    }

    protected function generateNewSession($data = '') {
      $this->gc(1);

      $encryption = new \HC\Encryption();
      $sessionID = $encryption->hash(uniqid(true) . mt_rand(0, 1000) . microtime(), ['salt' => uniqid(true) . mt_rand(0, 1000) . microtime(), 'hashlength' => 0]);

      $result = $this->connection->query('INSERT INTO `sessions` (`hash`, `lifeTime`, `data`) VALUES (?,?,?);', [$sessionID, (time() + self::$lifetime), $data]);
      if($result) {
        session_id($sessionID);
        setcookie(session_name(), $sessionID, time() + 63072000, '/', NULL, true);
        header('Location: /timeout', true, 302);
        return true;
      }

      return false;
    }
}
