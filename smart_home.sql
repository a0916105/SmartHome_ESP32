-- --------------------------------------------------------
-- 主機:                           127.0.0.1
-- 伺服器版本:                        10.5.6-MariaDB - mariadb.org binary distribution
-- 伺服器作業系統:                      Win64
-- HeidiSQL 版本:                  11.1.0.6116
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


-- 傾印 smart_home 的資料庫結構
CREATE DATABASE IF NOT EXISTS `smart_home` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci */;
USE `smart_home`;

-- 傾印  資料表 smart_home.home000 結構
CREATE TABLE IF NOT EXISTS `home000` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `schedule` enum('enable','disable') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'enable',
  `devices` int(10) unsigned NOT NULL,
  `Date` date DEFAULT NULL,
  `weekday` set('Sun','Mon','Tue','Wed','Thu','Fri','Sat','Only','Now') COLLATE utf8_unicode_ci DEFAULT NULL,
  `Time` time DEFAULT NULL,
  `switch` enum('On','Off') COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=61 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- 正在傾印表格  smart_home.home000 的資料：~22 rows (近似值)
/*!40000 ALTER TABLE `home000` DISABLE KEYS */;
INSERT INTO `home000` (`id`, `schedule`, `devices`, `Date`, `weekday`, `Time`, `switch`) VALUES
	(0, 'disable', 0, NULL, 'Now', NULL, 'Off'),
	(1, 'disable', 1, NULL, 'Now', NULL, 'Off'),
	(2, 'disable', 2, NULL, 'Now', NULL, 'On'),
	(13, 'enable', 1, NULL, 'Only', '17:05:00', 'On'),
	(15, 'enable', 1, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '23:25:00', 'Off'),
	(24, 'disable', 0, '2020-12-23', NULL, '14:24:00', 'On'),
	(25, 'enable', 1, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '14:59:00', 'On'),
	(29, 'enable', 1, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '19:38:00', 'Off'),
	(31, 'enable', 1, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '14:54:00', 'Off'),
	(33, 'enable', 2, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '15:46:00', 'On'),
	(35, 'enable', 1, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '17:09:00', 'On'),
	(43, 'enable', 2, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '20:02:00', 'On'),
	(46, 'enable', 2, NULL, 'Sun,Sat', '11:01:00', 'On'),
	(47, 'enable', 2, NULL, 'Mon,Fri,Sat', '11:01:00', 'On'),
	(48, 'disable', 2, NULL, 'Mon,Tue,Wed,Thu,Fri,Sat', '17:35:00', 'On'),
	(55, 'enable', 0, '2020-12-22', NULL, '14:12:00', 'On'),
	(56, 'enable', 1, NULL, 'Mon,Tue', '14:25:00', 'On'),
	(57, 'enable', 1, NULL, 'Mon,Tue', '16:25:00', 'Off'),
	(58, 'disable', 2, '2020-12-23', NULL, '13:40:00', 'Off'),
	(59, 'enable', 2, NULL, 'Sun,Tue,Thu', '17:10:00', 'On'),
	(60, 'enable', 0, NULL, 'Sun,Mon,Tue,Wed,Thu,Fri,Sat', '14:25:00', 'Off');
/*!40000 ALTER TABLE `home000` ENABLE KEYS */;

-- 傾印  資料表 smart_home.home000_sensors 結構
CREATE TABLE IF NOT EXISTS `home000_sensors` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `celsius` float NOT NULL,
  `humidity` float unsigned NOT NULL,
  `PM2_5` float unsigned NOT NULL,
  `datetime` datetime NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=162 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- 正在傾印表格  smart_home.home000_sensors 的資料：~155 rows (近似值)
/*!40000 ALTER TABLE `home000_sensors` DISABLE KEYS */;
INSERT INTO `home000_sensors` (`id`, `celsius`, `humidity`, `PM2_5`, `datetime`) VALUES
	(1, 25.7, 52, 10.8, '2020-10-31 21:44:22'),
	(2, 26.1, 57, 11.8, '2020-11-01 17:55:12'),
	(3, 28.45, 77.81, 12.8, '2020-11-01 19:44:51'),
	(4, 28.9, 79.6, 12.8, '2020-11-01 19:44:57'),
	(5, 29.32, 80.98, 12.8, '2020-11-01 19:45:02'),
	(6, 29.62, 82.08, 12.8, '2020-11-01 19:45:08'),
	(7, 28.64, 85.99, 12.8, '2020-11-01 20:19:55'),
	(8, 26.1, 59, 0, '2020-11-01 21:22:59'),
	(9, 26.1, 59, 0, '2020-11-01 21:23:05'),
	(10, 26.1, 59, 0, '2020-11-01 21:37:41'),
	(11, 26.1, 59, 0, '2020-11-01 21:37:47'),
	(12, 26.1, 59, 0, '2020-11-01 21:37:53'),
	(13, 26.1, 59, 1.9, '2020-11-01 21:37:58'),
	(14, 26.1, 59, 0, '2020-11-01 21:38:04'),
	(15, 26.1, 59, 7.62, '2020-11-01 21:38:11'),
	(16, 26.1, 59, 7.62, '2020-11-01 21:38:16'),
	(17, 26.1, 59, 7.62, '2020-11-01 21:38:22'),
	(18, 26.1, 59, 1.9, '2020-11-01 21:44:16'),
	(19, 26.1, 59, 1.9, '2020-11-01 21:44:21'),
	(20, 26.1, 59, 1.9, '2020-11-01 21:44:26'),
	(21, 26.1, 59, 5.71, '2020-11-01 21:44:34'),
	(22, 26.1, 59, 1.9, '2020-11-01 21:44:40'),
	(23, 26.1, 59, 1.9, '2020-11-01 21:44:46'),
	(24, 26.1, 59, 1.9, '2020-11-01 21:44:51'),
	(25, 26.1, 59, 1.9, '2020-11-01 21:44:57'),
	(26, 26.1, 59, 1.9, '2020-11-01 21:45:03'),
	(27, 26.1, 59, 1.9, '2020-11-01 21:45:08'),
	(28, 27.68, 82.35, 1.9, '2020-11-01 21:57:32'),
	(29, 30.18, 81.8, 1.9, '2020-11-01 21:58:03'),
	(30, 29.02, 83.43, 1.9, '2020-11-01 22:04:42'),
	(31, 31.34, 75.52, 1.9, '2020-11-01 22:05:43'),
	(32, 33.64, 72.25, 7.62, '2020-11-01 22:13:34'),
	(33, 35.09, 70.47, 11.43, '2020-11-01 22:14:06'),
	(34, 31.94, 62.1, 34.28, '2020-11-01 22:39:37'),
	(35, 31.67, 63.9, 30.47, '2020-11-01 22:40:08'),
	(36, 27.8, 88.62, 1.9, '2020-11-08 17:50:25'),
	(37, 28.85, 84.22, 5.71, '2020-11-08 17:50:56'),
	(38, 25.83, 79.88, 0, '2020-11-09 21:36:20'),
	(39, 30.35, 97.48, 0, '2020-11-09 21:36:52'),
	(40, 31.39, 90.71, 0, '2020-11-09 21:37:22'),
	(41, 31.46, 80.76, 0, '2020-11-09 21:37:53'),
	(42, 31.96, 83.18, 0, '2020-11-09 21:38:25'),
	(43, 26.93, 80.25, 0, '2020-11-09 22:52:55'),
	(44, 29.09, 91.96, 30.47, '2020-11-09 23:18:44'),
	(45, 29.2, 88.35, 5.71, '2020-11-10 22:49:53'),
	(46, 25.79, 80.83, 5.71, '2020-11-10 22:53:55'),
	(47, 27.91, 93.53, 11.43, '2020-11-14 22:49:35'),
	(48, 28.56, 89.84, 30.47, '2020-11-18 20:57:07'),
	(49, 29.73, 94.22, 5.71, '2020-11-18 21:42:01'),
	(50, 29.58, 87.34, 1.9, '2020-11-18 21:44:32'),
	(52, 30.55, 88.04, 22.85, '2020-11-21 16:44:24'),
	(53, 31.34, 86.7, 11.43, '2020-11-21 16:44:55'),
	(54, 30.48, 82.86, 3.81, '2020-11-21 16:46:07'),
	(55, 29.66, 81.6, 5.71, '2020-11-22 00:05:06'),
	(56, 29.42, 87.82, 41.89, '2020-11-22 14:45:40'),
	(57, 23.62, 75.82, 28.88, '2020-11-29 21:29:00'),
	(58, 23.67, 75.84, 34.91, '2020-11-29 21:39:01'),
	(59, 23.83, 75.33, 24.28, '2020-11-29 21:49:01'),
	(60, 24.16, 75.01, 18.25, '2020-11-29 21:59:02'),
	(61, 20.3, 70, 25.5, '2020-12-03 20:31:55'),
	(62, 20.7, 70.8, 20.5, '2020-12-03 20:43:01'),
	(63, 20.46, 84.92, 68.55, '2020-12-04 21:30:31'),
	(64, 20.31, 84.94, 104.74, '2020-12-04 21:40:32'),
	(65, 20.39, 85.46, 62.52, '2020-12-04 21:50:35'),
	(66, 20.41, 84.98, 41.74, '2020-12-04 22:02:06'),
	(67, 20.13, 86.3, 42.05, '2020-12-04 22:12:07'),
	(68, 20.47, 84.85, 43.32, '2020-12-04 23:38:36'),
	(69, 20.21, 84.33, 24.44, '2020-12-05 00:36:08'),
	(70, 19.9, 85.79, 3.65, '2020-12-05 10:21:15'),
	(71, 19.98, 85.55, 12.85, '2020-12-05 10:31:16'),
	(72, 19.95, 85.57, 12.06, '2020-12-05 10:41:16'),
	(73, 20.01, 85.35, 73.95, '2020-12-05 10:51:16'),
	(74, 19.83, 85.63, 35.39, '2020-12-05 11:01:17'),
	(75, 20.01, 85.1, 36.66, '2020-12-05 11:11:17'),
	(76, 20.11, 84.74, 38.09, '2020-12-05 11:21:17'),
	(77, 20.11, 84.71, 36.5, '2020-12-05 11:31:18'),
	(78, 20.07, 84.56, 59.03, '2020-12-05 11:43:22'),
	(79, 20.16, 84.41, 26.5, '2020-12-05 11:53:22'),
	(80, 20.03, 85.02, 18.57, '2020-12-05 12:03:22'),
	(81, 19.8, 85.67, 23.96, '2020-12-05 12:13:23'),
	(82, 20.12, 84.55, 46.5, '2020-12-05 12:23:24'),
	(83, 20.05, 85.17, 48.24, '2020-12-05 12:33:25'),
	(84, 20.03, 85.27, 51.57, '2020-12-05 12:43:25'),
	(85, 20.14, 85.05, 47.61, '2020-12-05 12:53:26'),
	(86, 20.14, 85.02, 55.38, '2020-12-05 13:03:27'),
	(87, 19.89, 85.72, 59.51, '2020-12-05 13:13:28'),
	(88, 20.06, 85.32, 9.2, '2020-12-05 14:59:27'),
	(89, 19.95, 85.63, 7.62, '2020-12-05 15:09:29'),
	(90, 19.99, 85.6, 7.62, '2020-12-05 15:19:29'),
	(91, 20.24, 85.03, 6.98, '2020-12-05 15:29:30'),
	(92, 20.13, 85.13, 39.04, '2020-12-05 15:39:30'),
	(93, 20.06, 85.4, 53.64, '2020-12-05 15:49:31'),
	(94, 20.06, 85.43, 41.1, '2020-12-05 15:59:31'),
	(95, 20.04, 85.31, 37.29, '2020-12-05 16:09:31'),
	(96, 19.94, 86.01, 34.44, '2020-12-05 16:44:24'),
	(97, 20.16, 84.93, 32.53, '2020-12-05 16:54:25'),
	(98, 20.01, 85.6, 4.76, '2020-12-05 17:04:25'),
	(99, 20.04, 85.5, 4.92, '2020-12-05 17:14:25'),
	(100, 19.95, 85.69, 5.71, '2020-12-05 17:24:26'),
	(101, 20.06, 85.53, 4.28, '2020-12-05 17:34:27'),
	(102, 19.92, 85.9, 6.82, '2020-12-05 17:44:27'),
	(103, 19.91, 86.14, 17.77, '2020-12-05 17:54:28'),
	(104, 20.2, 85.24, 4.92, '2020-12-05 19:37:24'),
	(105, 20.07, 85.77, 5.24, '2020-12-05 19:47:25'),
	(106, 19.87, 86.83, 4.6, '2020-12-05 19:57:25'),
	(107, 20.1, 85.95, 5.4, '2020-12-05 20:07:25'),
	(108, 19.82, 86.55, 3.33, '2020-12-05 20:17:26'),
	(109, 19.92, 86.85, 3.97, '2020-12-05 20:27:26'),
	(110, 20.07, 86.21, 5.71, '2020-12-05 20:37:27'),
	(111, 20.05, 86.05, 6.19, '2020-12-05 20:47:27'),
	(112, 19.98, 86.25, 4.44, '2020-12-05 20:57:28'),
	(113, 19.83, 88.24, 30.94, '2020-12-06 08:51:37'),
	(114, 20.76, 88.61, 7.46, '2020-12-06 15:12:11'),
	(115, 20.81, 88.71, 15.55, '2020-12-06 16:03:17'),
	(116, 20.77, 88.95, 43.96, '2020-12-06 16:13:17'),
	(117, 20.77, 88.94, 100.93, '2020-12-06 16:23:18'),
	(118, 20.85, 88.76, 101.56, '2020-12-06 16:33:18'),
	(119, 20.81, 89.33, 98.71, '2020-12-06 16:43:18'),
	(120, 20.84, 88.96, 107.28, '2020-12-06 16:53:19'),
	(121, 20.93, 88.76, 103.15, '2020-12-19 17:03:19'),
	(122, 20.96, 88.96, 14.12, '2020-12-19 17:26:25'),
	(123, 19.33, 97.95, 61.1, '2020-12-19 11:25:58'),
	(124, 19.31, 97.79, 42.05, '2020-12-19 11:35:59'),
	(125, 19.36, 97.51, 41.89, '2020-12-19 11:46:00'),
	(126, 19.32, 97.69, 32.37, '2020-12-19 11:56:00'),
	(127, 19.32, 97.59, 20.31, '2020-12-19 12:06:01'),
	(128, 19.33, 97.21, 29.83, '2020-12-19 12:25:39'),
	(129, 19.06, 96.21, 10.16, '2020-12-19 19:57:50'),
	(130, 19.09, 96.5, 7.14, '2020-12-19 20:07:51'),
	(131, 19.13, 96.44, 9.84, '2020-12-19 20:20:26'),
	(132, 19.09, 96.27, 12.7, '2020-12-19 20:30:27'),
	(133, 19, 96.35, 7.93, '2020-12-19 20:55:04'),
	(134, 19.08, 96.12, 9.36, '2020-12-19 21:11:36'),
	(135, 19.17, 95.63, 7.62, '2020-12-19 21:21:36'),
	(136, 19.13, 95.55, 8.73, '2020-12-19 21:39:21'),
	(137, 18.92, 95.85, 24.28, '2020-12-19 21:59:51'),
	(138, 18.95, 95.56, 26.82, '2020-12-19 22:09:53'),
	(139, 18.97, 95.56, 24.28, '2020-12-19 22:19:54'),
	(140, 19.33, 95.43, 20.63, '2020-12-19 22:29:55'),
	(141, 19.27, 95.62, 14.12, '2020-12-19 22:39:55'),
	(142, 19.1, 96.63, 22.69, '2020-12-19 22:49:57'),
	(143, 19.04, 96.41, 28.41, '2020-12-20 22:59:58'),
	(145, 19.27, 93.69, 131.71, '2020-12-20 21:15:14'),
	(146, 19.23, 94.28, 148.22, '2020-12-20 21:32:13'),
	(147, 19.25, 94.21, 75.06, '2020-12-20 21:42:13'),
	(148, 19.28, 94.48, 19.84, '2020-12-20 21:52:14'),
	(149, 19.9, 92.8, 47.61, '2020-12-20 22:02:16'),
	(150, 19.43, 92.93, 40.63, '2020-12-20 22:12:16'),
	(151, 19.28, 94.48, 53.32, '2020-12-20 22:22:17'),
	(152, 20.18, 91.41, 89.18, '2020-12-20 22:32:17'),
	(153, 20.28, 91.67, 66.17, '2020-12-20 22:42:48'),
	(154, 20.04, 91.9, 71.89, '2020-12-20 22:52:48'),
	(155, 20.1, 91.92, 117.27, '2020-12-20 23:02:49'),
	(156, 23.92, 74.35, 3.81, '2020-12-22 14:14:41'),
	(157, 23.74, 74.72, 5.71, '2020-12-22 14:15:17'),
	(158, 24.93, 80.99, 3.97, '2020-12-23 13:23:06'),
	(159, 25.6, 82.69, 1.9, '2020-12-23 13:24:38'),
	(160, 25.6, 80.08, 4.44, '2020-12-23 13:33:06'),
	(161, 23.18, 82.6, 9.84, '2020-12-23 14:05:16');
/*!40000 ALTER TABLE `home000_sensors` ENABLE KEYS */;

-- 傾印  資料表 smart_home.home000_settings 結構
CREATE TABLE IF NOT EXISTS `home000_settings` (
  `item` varchar(50) NOT NULL,
  `Auto` enum('Y','N') NOT NULL,
  `Celsius_On` float DEFAULT NULL,
  `Celsius_Off` float DEFAULT NULL,
  `Humidity_On` float unsigned DEFAULT NULL,
  `Humidity_Off` float unsigned DEFAULT NULL,
  `PM2_5_On` float unsigned DEFAULT NULL,
  `PM2_5_Off` float unsigned DEFAULT NULL,
  `AC` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `DH` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `AP` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`item`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 正在傾印表格  smart_home.home000_settings 的資料：~1 rows (近似值)
/*!40000 ALTER TABLE `home000_settings` DISABLE KEYS */;
INSERT INTO `home000_settings` (`item`, `Auto`, `Celsius_On`, `Celsius_Off`, `Humidity_On`, `Humidity_Off`, `PM2_5_On`, `PM2_5_Off`, `AC`, `DH`, `AP`) VALUES
	('set_values', 'Y', 13, NULL, NULL, 60, 10, NULL, 1, 1, 1);
/*!40000 ALTER TABLE `home000_settings` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
