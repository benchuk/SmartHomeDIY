https://stackoverflow.com/questions/12015598/how-to-set-unset-a-bit-at-specific-position-of-a-long

#Bits operations
### Set a bit:
```c
x |= 0b1; // set LSB bit
x |= 0b10; // set 2nd bit from LSB
```
### Erase a bit:
```c
x &= ~0b1; // unset LSB bit (if set)
x &= ~0b10; // unset 2nd bit from LSB
```
### Toggle a bit:
```c
x ^= 0b1;
```
> [!IMPORTANT]
> successNotice I use 0b?. You can also use any integer, eg:

```c
x |= 4; // sets 3rd bit
x |= 0x4; // sets 3rd bit
x |= 0x10; // sets 9th bit
```

However, it makes it harder to know which bit is being changed.

Using binary allows you to see which exact bits will be set/erased/toggled.

### Dynamically set at bit:

```c
x |= (1 << y); // set the yth bit from the LSB
```
> [!TIP]
> (1 << y) shifts the ...001 y places left, so you can move the set bit y places.

### Set multiple bits at once:
```c
x |= (1 << y) | (1 << z); // set the yth and zth bit from the LSB
```
#### Unset:
```c
x &= ~((1 << y) | (1 << z)); // unset yth and zth bit
```
#### Toggle:
```c
x ^= (1 << y) | (1 << z); // toggle yth and zth bit
```
