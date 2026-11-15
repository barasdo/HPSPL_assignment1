# מבצע B7 — דוח חקירה

**אל:** שירות הביטחון הכללי (שב״כ) ועיריית באר שבע  
**סטודנט/ית 1 — שם מלא:** ____דורון בראס________________  
**מספר זהות:** _____________322754169_______  
**סטודנט/ית 2 — שם מלא (אם יש):** _________יעל ברימר___________  
**מספר זהות (אם יש):** _______211876008_____________  
**תאריך:** _______14.11.2026_____________

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

Agent 67 is not in the official agent database, but he got access 8 times without permission. He reached BGU_CAMERAS and CONTROL_SYSTEM, and also Evil Morty's private communications. This means someone used a fake agent account to get into the systems, and Evil Morty is protecting it. Other events were normal: Greta Thunberg was denied 7 times on PORT_8080 and Mrs. Nora Beady was denied on BGU_CAMERAS, and both denials were correct.

Access code: `B7-M2-CUQHZRGO`

### הבנה טכנית

1. מדוע `find_agent` מחזירה מצביע לאיבר המקורי במערך ולא כתובת של משתנה מקומי?
2. מדוע אפשר לחשב את מספר האיברים במערך `working` באמצעות `sizeof` מתוך `main.c`, אבל לא מתוך הפרמטר `dest` של `copy_agents`?

**תשובת הצוות:**

1. A local variable is deleted when the function ends, so its address would be wrong after that. The original array is still alive, so the pointer stays valid. Also, the caller can change the real agent through it.

2. In `main.c`, `working` is a real array, so `sizeof` gives its full size and we can divide by the size of one element. In `copy_agents`, `dest` is only a pointer, so `sizeof(dest)` gives the size of a pointer (8 bytes) and not the size of the array. That is why the capacity is passed as a parameter.

---

## משימה 3 — הגיבוי שפלנקטון שכח

### ממצאים

מה למדתם מההתכתבויות ששוחזרו, וכיצד הן משפיעות על המשך המבצע? ציינו את קוד ההשלמה בפורמט `B7-M3-XXXXXXXX`.

**תשובת הצוות:**

The Beershevanator will start at midnight. It will attack traffic control, access systems and city infrastructure, and Iran is paying for it. Doofenshmirtz connected a self-destruct button to the server, so we may be able to stop it remotely. Agent 67 told Evil Morty that his communication channel works on its own, and Evil Morty said the others should not know. Next step: fix the emergency controller before midnight.

Completion code: `B7-M3-CDRAUGLI`

### הבנה טכנית

1. הבחינו בין **קלט מושאל** לבין זיכרון שבבעלות `MessageStore`: מה מותר לשחרר ומי אחראי לכך?
2. נניח שהקצאת זיכרון נכשלת באמצע `store_clone`. מה חייב לקרות להקצאות שכבר בוצעו ולמצב של `dest`, ומדוע?

**תשובת הצוות:**

1. A borrowed string (`const char *`) belongs to someone else. We can read it and copy it, but we must not free it or keep its address. The `MessageStore` owns its array and the text of every message, because we made our own copies. So the store is the one that frees them, with `store_free`.

2. We must free everything that was already allocated and leave `dest` empty. If we don't, the memory is lost (a leak) and `dest` is left half-built. In our code, `store_free(dest)` does this.

---

## משימה 4 — הבקר השבור של פלנקטון

### ממצאים

מה הצלחתם לשחזר בבקר, ומה עדיין מונע את השלמת הפעולה? ציינו את קוד ההשלמה בפורמט `B7-M4-XXXXXXXX`.

**תשובת הצוות:**

We restored the emergency controller and the self-destruct interface. The required order is to prepare the emergency command, request authorization, and then execute it. The controller is working, but valid authorization is still missing, so the command was not executed.

Access code: `B7-M4-DZTZXGTR`

### תיעוד התיקונים

**לכל אחד מהשלבים A–D**, תעדו בקצרה: מה היה שורש התקלה, מה שיניתם ומדוע התיקון עובד, ואיזו בדיקה מאמתת אותו (קלט או תרחיש ותוצאה צפויה). מספיקות כמה שורות לכל שלב.

**שלב A:**

The original check used `length > capacity`, so it allowed copying when the text length equaled the buffer size, leaving no room for `'\0'`. We changed `>` to `>=`. For example, `"STOP"` needs 5 bytes, so a buffer of size 4 must be rejected and left empty. `make test-a` failed before the fix and passed afterward.

**שלב B:**

The original check used `index > count`, so it accepted `index == count`, which is outside the valid range. We changed `>` to `>=`. For an array of 3 elements, index 3 must return `NULL`, while index 2 must return a pointer to the original element. `make test-b` failed before the fix and passed afterward.

**שלב C:**

The original code passed `remaining` to `memmove`, but this is the number of commands, while `memmove` needs the number of bytes. We changed it to `remaining * sizeof(*commands)` so it copies whole commands. For example, removing B from `[A, B, C, D]` should leave `[A, C, D]`, reduce the count to 3, and clear the last slot. `make test-c` failed before the fix and passed afterward.

**שלב D:**

The original code freed the command's text at the end of `controller_schedule`, even though it was still needed later. We removed this `free` and kept the one in `controller_execute_pending`, after the text is copied successfully. A scheduled command should keep its text until execution, and successful execution should free the command and set the pending pointer to `NULL`. `make test-d` failed before the fix and passed afterward.

### חקירת GDB אחת

בחרו **שלב אחד בלבד** מבין A–D. היכן עצרתם, באילו פקודות עיקריות השתמשתם, מה ראיתם בפועל וכיצד התצפית עזרה לזהות את התקלה? אין צורך בתמליל או בצילום מסך.

**תשובת הצוות:**

We used GDB to stop in `controller_find_unit` when `index == count`. We used `break` to set the breakpoint and `run` to start the test. With `info args`, we saw that `count` and `index` were both 3. Using `print`, we checked that `index > count` was false, while `index >= count` was true. Then we used `finish` and saw that the function returned a pointer instead of `NULL`. This helped us understand why the check needed to be `>=`.

---

## משימה 5 — הפקודה האחרונה

### ממצאים

מה גיליתם על תהליך ההרשאה, מה הייתה תוצאת המבצע ומה נשאר לא ידוע בסיום החקירה? ציינו את קוד ההשלמה בפורמט `B7-M5-XXXXXXXX`. התבססו רק על המידע שנחשף בהרצה.

**תשובת הצוות:**

We got authorization through PORT_8080. The final messages revealed that Plankton had given Greta administrator privileges and left the emergency authorization interface connected to her session. We used this authorization to submit the request. The Beershevanator was destroyed and Be'er Sheva was saved.

We also learned that the attack was a distraction for Agent 67's attempt to access a classified project at Ben-Gurion University. It is still unknown whether the attempt succeeded or what information was exposed. Evil Morty is still at large, and Agent 67's real identity is unknown.

Completion code: `B7-M5-RLBZFXAM`

### הבנה טכנית

1. מה ההבדל בין בקשת חירום שממתינה לאישור לבין בקשה שאושרה, ומדוע לא מספיק לשנות את שדה האישור ידנית?
2. אילו נתונים בבקשה חייבים להישמר בעותק עצמאי, ומה עלול לקרות אם נשמרים רק מצביעים למאגר הראיות?
3. תארו **מקרה כשל אחד** שבו אסור לשלוח את פקודת הביצוע, והסבירו מה בקוד מונע זאת.

**תשובת הצוות:**

1. A pending request has `approved == 0`, while an approved request has `approved == 1`. Approval must come through `mission5_authorize_via_port`. Changing the field ourselves would skip the authorization process. The authorization record is copied by value into the request, so changing this copy does not change the original record in the evidence.

2. The device name and the steps are copied into separately allocated memory. The mode and action are copied into the request's arrays, and the authorization record is copied by value. If we only saved pointers to the evidence, changing or freeing the evidence could change the request or leave it with invalid pointers.

3. A request with steps in the wrong order must not be submitted. `validate_emergency_request` checks that there are exactly three steps in the order prepare, request authorization, and execute. If the check fails, `main` goes to cleanup without submitting the request. Without this check, execution could be requested before authorization.
