# Architecture

This project uses a layered embedded structure:

```text
app -> services -> board -> bsp -> drivers
app -> middleware/components/osal
startup -> osal
```
