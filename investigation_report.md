# מבצע B7 — דוח חקירה

**אל:** שירות הביטחון הכללי (שב״כ) ועיריית באר שבע  
**סטודנט/ית 1 — שם מלא:** ____________________  
**מספר זהות:** ____________________  
**סטודנט/ית 2 — שם מלא (אם יש):** ____________________  
**מספר זהות (אם יש):** ____________________  
**תאריך:** ____________________

> זהו דוח פרטי המוגש לצוות הקורס. מספרי הזהות משמשים לאימות קודי הגישה האישיים, ומותר לכלול אותם רק בדוח זה — לא בקוד המקור, בקובצי הראיות או בפלט התוכניות.
>
> ענו **בקצרה ובמילים שלכם**. אין צורך לתאר מחדש את הקוד או להעתיק את הוראות המשימה; התמקדו במה שגיליתם ובהחלטות הטכניות החשובות. אין להדביק פלט תוכנית מלא או צילומי מסך. תשובה קצרה ומנומקת מספיקה, ואין דרישת מינימום לאורך.
>
> מלאו כל פרק **רק לאחר השלמת המשימה המתאימה והרצתה בהצלחה**, כדי להסתמך על הממצאים שנחשפו בפועל.

## משימה 1 — השידור המיורט

### ממצאים

מה גיליתם בתשדורות, ומהו הממצא החשוב ביותר להמשך החקירה? ציינו את קוד הגישה בפורמט `B7-M1-XXXXXXXX`.

**תשובת הצוות:**

Out of 18 transmissions, 16 were valid and 2 were rejected. The transmissions show that Doofenshmirtz is planning "Operation Beershevanator" and wants the complete agent list before the machine starts. Plankton admits he allocated space for too few agents, and Evil Morty orders the team to hide agent 67 from the official report and tells them not to ask why the agent count and the report disagree. This is the most important finding for the next step: an agent is being hidden from the official records, so the investigation continues in the agent database. The other messages (Greta Thunberg looking for a seaport in Beer Sheva, the suspicious camels) are noise.

Access code: `B7-M1-DWEEXECU`

### הבנה טכנית

הביאו **דוגמה אחת** לרשומה פגומה (אמיתית מהראיות או דוגמה שיצרתם), והסבירו מה גורם ל־`process_transmission` לדחות אותה. אין צורך לתאר את אלגוריתם הפענוח כולו.

**תשובת הצוות:**

Record from the evidence: `CONTROL|25|23|FRPD_NVS_SQZO_DCNB_RRDBBZ`

Its structure is valid: it has exactly three separators, the sender and key are legal, and the message decodes without errors to `ACCESS_CODE_PART_TWO_EQSG`. However, the sum of the ASCII values of the decoded message modulo 97 is 22, while the record says the checksum is 23. Because of this mismatch, `process_transmission` prints `[INVALID]` and returns 0. This is exactly the case the checksum is meant to catch: the record looks legitimate and even decodes, but its content was forged or corrupted, so it cannot be trusted for extracting the code.

---

## משימה 2 — הסוכן שלא קיים

### ממצאים

אילו אירועי גישה חריגים גיליתם, ומה אפשר להסיק מהם על סוכן 67? ציינו את קוד הגישה בפורמט `B7-M2-XXXXXXXX`.

**תשובת הצוות:**

<!-- כתבו כאן. -->

### הבנה טכנית

1. מדוע `find_agent` מחזירה מצביע לאיבר המקורי במערך ולא כתובת של משתנה מקומי?
2. מדוע אפשר לחשב את מספר האיברים במערך `working` באמצעות `sizeof` מתוך `main.c`, אבל לא מתוך הפרמטר `dest` של `copy_agents`?

**תשובת הצוות:**

<!-- כתבו כאן. -->

---

## משימה 3 — הגיבוי שפלנקטון שכח

### ממצאים

מה למדתם מההתכתבויות ששוחזרו, וכיצד הן משפיעות על המשך המבצע? ציינו את קוד ההשלמה בפורמט `B7-M3-XXXXXXXX`.

**תשובת הצוות:**

<!-- כתבו כאן. -->

### הבנה טכנית

1. הבחינו בין **קלט מושאל** לבין זיכרון שבבעלות `MessageStore`: מה מותר לשחרר ומי אחראי לכך?
2. נניח שהקצאת זיכרון נכשלת באמצע `store_clone`. מה חייב לקרות להקצאות שכבר בוצעו ולמצב של `dest`, ומדוע?

**תשובת הצוות:**

<!-- כתבו כאן. -->

---

## משימה 4 — הבקר השבור של פלנקטון

### ממצאים

מה הצלחתם לשחזר בבקר, ומה עדיין מונע את השלמת הפעולה? ציינו את קוד ההשלמה בפורמט `B7-M4-XXXXXXXX`.

**תשובת הצוות:**

<!-- כתבו כאן. -->

### תיעוד התיקונים

**לכל אחד מהשלבים A–D**, תעדו בקצרה: מה היה שורש התקלה, מה שיניתם ומדוע התיקון עובד, ואיזו בדיקה מאמתת אותו (קלט או תרחיש ותוצאה צפויה). מספיקות כמה שורות לכל שלב.

**שלב A:**

<!-- תקלה | תיקון | בדיקה -->

**שלב B:**

<!-- תקלה | תיקון | בדיקה -->

**שלב C:**

<!-- תקלה | תיקון | בדיקה -->

**שלב D:**

<!-- תקלה | תיקון | בדיקה -->

### חקירת GDB אחת

בחרו **שלב אחד בלבד** מבין A–D. היכן עצרתם, באילו פקודות עיקריות השתמשתם, מה ראיתם בפועל וכיצד התצפית עזרה לזהות את התקלה? אין צורך בתמליל או בצילום מסך.

**תשובת הצוות:**

<!-- כתבו כאן. -->

---

## משימה 5 — הפקודה האחרונה

### ממצאים

מה גיליתם על תהליך ההרשאה, מה הייתה תוצאת המבצע ומה נשאר לא ידוע בסיום החקירה? ציינו את קוד ההשלמה בפורמט `B7-M5-XXXXXXXX`. התבססו רק על המידע שנחשף בהרצה.

**תשובת הצוות:**

<!-- כתבו כאן. -->

### הבנה טכנית

1. מה ההבדל בין בקשת חירום שממתינה לאישור לבין בקשה שאושרה, ומדוע לא מספיק לשנות את שדה האישור ידנית?
2. אילו נתונים בבקשה חייבים להישמר בעותק עצמאי, ומה עלול לקרות אם נשמרים רק מצביעים למאגר הראיות?
3. תארו **מקרה כשל אחד** שבו אסור לשלוח את פקודת הביצוע, והסבירו מה בקוד מונע זאת.

**תשובת הצוות:**

<!-- כתבו כאן. -->
