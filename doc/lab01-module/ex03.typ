#import "/doc/metadata.typ": *

=== What does it mean the 4 values in ```/proc/sys/kernel/printk``` ?  <lab01:ex03>

We can show what there is in:

```bash
|> cat /proc/sys/kernel/printk
7	4	1	7
```
The number specified the level of output in a console.

This file specifies the log level for: \
current (7), default (4), minimum (1) and boot-time default (7).

This number matches with this table (#link("https://www.kernel.org/doc/html/latest/core-api/printk-basics.html", [printk documentation])):

#table(
  columns: (2fr, 1fr, 3fr),

  [*Name*], [*String*], [*Alias function*],

  [KERN_EMERG], ["0"], [pr_emerg()],
  [KERN_ALERT], ["1"], [pr_alert()],
  [KERN_CRIT], ["2"], [pr_crit()],
  [KERN_ERR], ["3"], [pr_err()],
  [KERN_WARNING], ["4"], [pr_warning()],
  [KERN_NOTICE], ["5"], [pr_notice()],
  [KERN_INFO], ["6"], [pr_info()],
  [KERN_DEBUG], ["7"], [pr_debug() and pr_devel() if DEBUG is defined],
  [KERN_DEFAULT], [""], [],
  [KERN_CONT], ["c"], [pr_cont()],
)
